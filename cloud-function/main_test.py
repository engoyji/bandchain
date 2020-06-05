from main import create_app
from flask import json
from test.support import EnvironmentVarGuard
import os
import pytest

def test_error_invalid_json_request():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data="{'executable': '123', 'calldata':}",
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "invalid JSON request format"

def test_error_missing_executable():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'calldata': 'bitcoin', 'timeout': 123456}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "executable field is missing from JSON request"

def test_error_executable_empty():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '', 'calldata': 'bitcoin', 'timeout': 123456}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 126
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Execution fail"

def test_error_missing_calldata():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '123', 'timeout': 123456}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "calldata field is missing from JSON request"


def test_error_calldata_empty():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': 'IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwpwcmludCgnaGVsbG8nKQ==', 'calldata': '123', 'timeout': 123456}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 0
  assert data['stdout'] == "hello\n"
  assert data['stderr'] == ""
  assert data['err'] == ""

def test_error_missing_timeout():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '123', 'calldata': 'bitcoin'}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "timeout field is missing from JSON request"

def test_error_timeout_empty():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '123', 'calldata': 'bitcoin', 'timeout': ''}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "timeout field is empty"

def test_error_timeout_less_than_0():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '123', 'calldata': 'bitcoin', 'timeout': -5}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "Runtime must more than 0"

def test_error_timeout_more_than_max_timeout():
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({'executable': '123', 'calldata': 'bitcoin', 'timeout': 1111111111111111111111111111111111111111}),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 400
  assert data['error'] == "Runtime exceeded max size"

def test_success_execution():
  '''#!/usr/bin/env python3
      print('hello')
  '''
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "123",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwpwcmludCgnaGVsbG8nKQ==",
        "timeout": 123456
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 0
  assert data['stdout'] == "hello\n"
  assert data['stderr'] == ""
  assert data['err'] == ""

def test_error_execution_fail():
  '''#!/usr/bin/enveeeeeeeee python3
      print('hello')
  '''
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnZlZWVlZWVlZWUKcHl0aG9uM1xucHJpbnQoJ2hlbGxvJyk=",
        "timeout": 123456
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 126
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Execution fail"

def test_error_execution_timeout():
  '''#!/usr/bin/env python3
      import time

      time.sleep(1)
  '''
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwppbXBvcnQgdGltZQoKdGltZS5zbGVlcCgxKQ==",
        "timeout": 100 #100 millisec
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 111
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Execution time limit exceeded"

def test_success_execution_timeout():
  '''#!/usr/bin/env python3
      import time

      time.sleep(1) # 1000 millisec
  '''
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwppbXBvcnQgdGltZQoKdGltZS5zbGVlcCgxKQpwcmludCgiaGVsbG8iKQ==",
        "timeout": 2000  # 2000 millisec
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 0
  assert data['stdout'] == "hello\n"
  assert data['stderr'] == ""
  assert data['err'] == ""

def test_error_infinite_loop_execution():
  '''#!/usr/bin/env python3
      import time

      while True:
          print("hello")
  '''
  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwppbXBvcnQgdGltZQoKd2hpbGUgVHJ1ZToKICAgIHByaW50KCJoZWxsbyIp",
        "timeout": 1000  # 1000 millisec
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 111
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Execution time limit exceeded"

@pytest.fixture
def mock_env(monkeypatch):
  monkeypatch.setenv("MAX_STDOUT", "1")
  monkeypatch.setenv("MAX_STDERR", "1")

def test_error_stdout_exceed(mock_env):
  '''#!/usr/bin/env python3
      import time

      while True:
          print("hello")
  '''

  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwppbXBvcnQgdGltZQoKZm9yIGkgaW4gcmFuZ2UoMTApOgogICAgcHJpbnQgKGkp",
        "timeout": 1000  # 1000 millisec
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 112
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Stdout exceeded max size"

def test_error_stderr_exceed(mock_env):
  '''#!/usr/bin/env python3
      import time

      print (1/0)
  '''

  app = create_app()
  response = app.test_client().post(
      '/execute',
      data=json.dumps({
        "calldata": "",
        "executable": "IyEvdXNyL2Jpbi9lbnYgcHl0aG9uMwppbXBvcnQgdGltZQoKcHJpbnQgKDEvMCk=",
        "timeout": 1000  # 1000 millisec
      }),
      content_type='application/json',
  )

  data = json.loads(response.get_data(as_text=True))
  assert response.status_code == 200
  assert data['returncode'] == 113
  assert data['stdout'] == ""
  assert data['stderr'] == ""
  assert data['err'] == "Stderr exceeded max size"


