module Styles = {
  open Css;

  let vFlex = style([display(`flex), flexDirection(`row), alignItems(`center)]);

  let logo = style([width(`px(50)), marginRight(`px(10))]);
  let logoSmall = style([width(`px(20))]);

  let fillLeft = style([marginLeft(`auto)]);

  let header =
    style([display(`flex), flexDirection(`row), alignItems(`center), height(`px(50))]);

  let fullWidth = dir => style([width(`percent(100.0)), display(`flex), flexDirection(dir)]);

  let seperatedLine =
    style([
      width(`px(13)),
      height(`px(1)),
      marginLeft(`px(10)),
      marginRight(`px(10)),
      backgroundColor(Colors.blueGray2),
    ]);

  let longLine =
    style([
      width(`percent(100.)),
      height(`px(2)),
      backgroundColor(Colors.blueGray2),
      marginTop(`px(30)),
      marginBottom(`px(45)),
    ]);

  let topPartWrapper =
    style([
      width(`percent(100.0)),
      display(`flex),
      flexDirection(`column),
      backgroundColor(Colors.white),
      borderRadius(`px(4)),
      padding2(~v=`px(35), ~h=`px(30)),
      boxShadow(Shadow.box(~x=`zero, ~y=`px(2), ~blur=`px(8), Css.rgba(0, 0, 0, 0.08))),
    ]);

  let underline = style([textDecoration(`underline), color(Colors.gray7)]);
};

type value_row_t =
  | VAddress(Address.t)
  | VValidatorAddress(Address.t)
  | VText(string)
  | VDetail(string)
  | VExtLink(string)
  | VCode(string)
  | Loading(int, int);

let kvRow = (k, description, v: value_row_t) => {
  <Row alignItems=`flexStart>
    <Col size=1.>
      <div className={Styles.fullWidth(`row)}>
        <Text value=k weight=Text.Thin tooltipItem=description tooltipPlacement=Text.AlignRight />
      </div>
    </Col>
    <Col size=1. justifyContent=Col.Center alignItems=Col.End>
      <div className={Styles.fullWidth(`row)}>
        <div className=Styles.fillLeft />
        {switch (v) {
         | VAddress(address) => <AddressRender address />
         | VValidatorAddress(address) => <AddressRender address validator=true clickable=false />
         | VText(value) => <Text value nowrap=true />
         | VDetail(value) => <Text value align=Text.Right />
         | VExtLink(value) =>
           <a href=value target="_blank" rel="noopener">
             <div className=Styles.underline> <Text value nowrap=true /> </div>
           </a>
         | VCode(value) => <Text value code=true nowrap=true />
         | Loading(width, height) => <LoadingCensorBar width height />
         }}
      </div>
    </Col>
  </Row>;
};

module Uptime = {
  [@react.component]
  let make = (~consensusAddress) => {
    let uptimeSub = ValidatorSub.getUptime(consensusAddress);
    switch (uptimeSub) {
    | Data(uptimeOpt) =>
      switch (uptimeOpt) {
      | Some(uptime) =>
        kvRow(
          "UPTIME (LAST 250 BLOCKS)",
          {
            "Percentage of the blocks that the validator is active for out of the last 250"
            |> React.string;
          },
          VCode(uptime |> Format.fPercent(~digits=2)),
        )
      | None =>
        kvRow(
          "UPTIME (LAST 250 BLOCKS)",
          {
            "Percentage of the blocks that the validator is active for out of the last 250"
            |> React.string;
          },
          VText("N/A"),
        )
      }
    | _ =>
      kvRow(
        "UPTIME (LAST 250 BLOCKS)",
        {
          "Percentage of the blocks that the validator is active for out of the last 250"
          |> React.string;
        },
        Loading(100, 16),
      )
    };
  };
};

[@react.component]
let make = (~address, ~hashtag: Route.validator_tab_t) => {
  let validatorSub = ValidatorSub.get(address);
  let bondedTokenCountSub = ValidatorSub.getTotalBondedAmount();

  let allSub = Sub.all2(validatorSub, bondedTokenCountSub);

  <>
    <Row justify=Row.Between>
      <Col>
        <div className={Css.merge([Styles.vFlex, Styles.header])}>
          <img src=Images.validatorLogo className=Styles.logo />
          <Text
            value="VALIDATOR DETAILS"
            weight=Text.Medium
            size=Text.Md
            spacing={Text.Em(0.06)}
            height={Text.Px(15)}
            nowrap=true
            color=Colors.gray7
            block=true
          />
          <div className=Styles.seperatedLine />
          {switch (allSub) {
           | Data((validator, _)) =>
             <>
               <Text
                 value={validator.isActive ? "ACTIVE" : "INACTIVE"}
                 size=Text.Md
                 weight=Text.Thin
                 spacing={Text.Em(0.06)}
                 color=Colors.gray7
                 nowrap=true
               />
               <HSpacing size=Spacing.md />
               <img
                 src={
                   validator.isActive ? Images.activeValidatorLogo : Images.inactiveValidatorLogo
                 }
                 className=Styles.logoSmall
               />
             </>
           | _ => <LoadingCensorBar width=80 height=24 />
           }}
          <HSpacing size=Spacing.md />
        </div>
      </Col>
    </Row>
    <VSpacing size=Spacing.xl />
    <div className=Styles.vFlex>
      {switch (allSub) {
       | Data((validator, _)) =>
         <>
           <Avatar moniker={validator.moniker} identity={validator.identity} width=40 />
           <HSpacing size=Spacing.md />
           <Text value={validator.moniker} size=Text.Xxl weight=Text.Bold nowrap=true />
         </>
       | _ => <LoadingCensorBar width=150 height=30 />
       }}
    </div>
    <VSpacing size=Spacing.xl />
    <ValidatorStakingInfo validatorAddress=address />
    <VSpacing size=Spacing.md />
    <div className=Styles.topPartWrapper>
      <Text value="INFORMATION" size=Text.Lg weight=Text.Semibold />
      <VSpacing size=Spacing.lg />
      {kvRow(
         "OPERATOR ADDRESS",
         {
           "The address used to show the entity's validator status" |> React.string;
         },
         {
           switch (allSub) {
           | Data(_) => VValidatorAddress(address)
           | _ => Loading(360, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "ADDRESS",
         {
           "The entity's unique address" |> React.string;
         },
         {
           switch (allSub) {
           | Data(_) => VAddress(address)
           | _ => Loading(310, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "VOTING POWER",
         {
           "Sum of self-bonded and delegated tokens" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, bondedTokenCount)) =>
             VCode(
               (
                 bondedTokenCount.amount > 0.
                   ? validator.votingPower *. 100. /. bondedTokenCount.amount : 0.
               )
               ->Format.fPretty
               ++ "% ("
               ++ (validator.votingPower /. 1e6 |> Format.fPretty)
               ++ " BAND)",
             )
           | _ => Loading(200, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "COMMISSION",
         {
           "Validator service fees charged to delegators" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) => VCode(validator.commission |> Format.fPercent(~digits=2))
           | _ => Loading(100, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "COMMISSION MAX CHANGE",
         {
           "Maximum increment in which the validator can change their commission rate at a time"
           |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) =>
             VCode(validator.commissionMaxChange |> Format.fPercent(~digits=2))
           | _ => Loading(100, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "COMMISSION MAX RATE",
         {
           "The highest possible commission rate the validator can set" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) =>
             VCode(validator.commissionMaxRate |> Format.fPercent(~digits=2))
           | _ => Loading(100, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "BONDED HEIGHT",
         {
           "The block height at which the entity registers as a validator" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) => VCode(validator.bondedHeight->Format.iPretty)
           | _ => Loading(100, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {switch (allSub) {
       | Data((validator, _)) => <Uptime consensusAddress={validator.consensusAddress} />

       | _ =>
         kvRow(
           "UPTIME",
           {
             "Percentage of the blocks that the validator is active for out of the last 250"
             |> React.string;
           },
           Loading(100, 16),
         )
       }}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "WEBSITE",
         {
           "The validator's website" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) => VExtLink(validator.website)
           | _ => Loading(100, 16)
           };
         },
       )}
      <VSpacing size=Spacing.lg />
      {kvRow(
         "DETAILS",
         {
           "Extra self-added detail about the validator" |> React.string;
         },
         {
           switch (allSub) {
           | Data((validator, _)) => VDetail(validator.details)
           | _ => Loading(100, 16)
           };
         },
       )}
    </div>
    // <div className=Styles.longLine />
    // <div className={Styles.fullWidth(`row)}>
    //   <Col size=1.>
    //     <Text value="NODE STATUS" size=Text.Lg weight=Text.Semibold />
    //     <VSpacing size=Spacing.lg />
    //     {kvRow("UPTIME", VCode(validator.nodeStatus.uptime->Format.fPretty ++ "%"))}
    //     <VSpacing size=Spacing.lg />
    //     {kvRow(
    //        "AVG. RESPONSE TIME",
    //        VCode(
    //          validator.avgResponseTime->Format.iPretty
    //          ++ (validator.avgResponseTime <= 1 ? " block" : " blocks"),
    //        ),
    //      )}
    //   </Col>
    //   <HSpacing size=Spacing.lg />
    //   <Col size=1.>
    //     <Text value="REQUEST RESPONSE" size=Text.Lg weight=Text.Semibold />
    //     <VSpacing size=Spacing.lg />
    //     {kvRow("COMPLETED REQUESTS", VCode(validator.completedRequestCount->Format.iPretty))}
    //     <VSpacing size=Spacing.lg />
    //     {kvRow("MISSED REQUESTS", VCode(validator.missedRequestCount->Format.iPretty))}
    //   </Col>
    // </div>
    <VSpacing size=Spacing.md />
    <Tab
      tabs=[|
        {
          name: "PROPOSED BLOCKS",
          route: Route.ValidatorIndexPage(address, Route.ProposedBlocks),
        },
        {name: "DELEGATORS", route: Route.ValidatorIndexPage(address, Route.Delegators)},
        {name: "REPORTS", route: Route.ValidatorIndexPage(address, Route.Reports)},
      |]
      currentRoute={Route.ValidatorIndexPage(address, hashtag)}>
      {switch (hashtag) {
       | ProposedBlocks =>
         switch (validatorSub) {
         | Data(validator) =>
           <ProposedBlocksTable consensusAddress={validator.consensusAddress} />
         | _ => <ProposedBlocksTable.LoadingWithHeader />
         }
       | Delegators => <DelegatorsTable address />
       | Reports => <ReportsTable address />
       }}
    </Tab>
  </>;
};
