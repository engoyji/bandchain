[@react.component]
let make = () => {
  let txs: list(TxHook.Tx.t) = [
    {
      sender: "0x498968C2B945Ac37b78414f66167b0786E522636" |> Address.fromHex,
      blockHeight: 120339,
      hash: "0x103020321239012391012300" |> Hash.fromHex,
      timestamp: MomentRe.momentWithUnix(1293912392),
      gasWanted: 0,
      gasUsed: 0,
      messages: [
        Report({
          requestId: 2,
          data: "0x88123812388231823180" |> JsBuffer.fromHex,
          validator: "0x9139329932193293192130" |> Address.fromHex,
        }),
      ],
      events: [],
    },
    {
      sender: "0x498968C2B945Ac37b78414f66167b0786E522636" |> Address.fromHex,
      blockHeight: 120338,
      hash: "0x123912912391239213921390" |> Hash.fromHex,
      timestamp: MomentRe.momentWithUnix(1293912392),
      gasWanted: 0,
      gasUsed: 0,
      messages: [
        Send({
          fromAddress: "0x923239239923923923" |> Address.fromHex,
          toAddress: "0x233262363262363263" |> Address.fromHex,
          amount: [{denom: "BAND", amount: 12.4}, {denom: "UATOM", amount: 10000.3}],
        }),
      ],
      events: [],
    },
    {
      sender: "0x498968C2B945Ac37b78414f66167b0786E522636" |> Address.fromHex,
      blockHeight: 120337,
      hash: "0x123912912391239213921390" |> Hash.fromHex,
      timestamp: MomentRe.momentWithUnix(1293912392),
      gasWanted: 0,
      gasUsed: 0,
      messages: [
        Store({
          code: "0x19239129123912932190" |> JsBuffer.fromHex,
          owner: "0x949494949499494949494" |> Address.fromHex,
        }),
      ],
      events: [],
    },
    {
      sender: "0x498968C2B945Ac37b78414f66167b0786E522636" |> Address.fromHex,
      blockHeight: 120337,
      hash: "0x123912912391239213921390" |> Hash.fromHex,
      timestamp: MomentRe.momentWithUnix(1293912392),
      gasWanted: 0,
      gasUsed: 0,
      messages: [
        Request({
          codeHash: "0x912381238128381230" |> Hash.fromHex,
          params: "0x8238233288238238" |> JsBuffer.fromHex,
          reportPeriod: 23,
          sender: "0x99329329239239923923" |> Address.fromHex,
        }),
      ],
      events: [],
    },
  ];
  <>
    <THead>
      <Row>
        <Col> <div className=TElement.Styles.msgIcon /> </Col>
        <Col size=1.1>
          <div className=TElement.Styles.hashContainer>
            <Text block=true value="TX HASH" size=Text.Sm weight=Text.Bold color=Colors.grayText />
          </div>
        </Col>
        <Col size=1.1>
          <Text block=true value="TYPE" size=Text.Sm weight=Text.Bold color=Colors.grayText />
        </Col>
        <Col size=0.65>
          <Text block=true value="BLOCK" size=Text.Sm weight=Text.Bold color=Colors.grayText />
        </Col>
        <Col size=1.1>
          <Text block=true value="SENDER" size=Text.Sm weight=Text.Bold color=Colors.grayText />
        </Col>
        <Col size=0.5>
          <div className=TElement.Styles.feeContainer>
            <Text block=true value="FEE" size=Text.Sm weight=Text.Bold color=Colors.grayText />
          </div>
        </Col>
      </Row>
    </THead>
    {txs
     ->Belt.List.mapWithIndex((idx, {blockHeight, hash, timestamp, gasUsed, messages}) => {
         <TBody key={idx |> string_of_int}>
           <Row>
             <Col> <TElement elementType={messages->Belt.List.getExn(0)->TElement.Icon} /> </Col>
             <Col size=1.1> <TElement elementType={TElement.TxHash(hash, timestamp)} /> </Col>
             <Col size=1.1>
               <TElement elementType={messages->Belt.List.getExn(0)->TElement.TxTypeWithDetail} />
             </Col>
             <Col size=0.65> <TElement elementType={TElement.Height(blockHeight)} /> </Col>
             <Col size=1.1> <TElement elementType={hash->TElement.Hash} /> </Col>
             <Col size=0.5> <TElement elementType={gasUsed->TElement.Fee} /> </Col>
           </Row>
         </TBody>
       })
     ->Array.of_list
     ->React.array}
  </>;
};
