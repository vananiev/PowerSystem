object frmScope: TfrmScope
  Left = 319
  Top = 190
  Width = 708
  Height = 406
  Caption = #1054#1089#1094#1080#1083#1086#1075#1088#1072#1092
  Color = clBtnFace
  Constraints.MinHeight = 100
  Constraints.MinWidth = 100
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnDestroy = FormDestroy
  OnHide = FormHide
  OnMouseWheel = FormMouseWheel
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Graph: TPaintBox
    Left = 8
    Top = -4
    Width = 673
    Height = 293
    Cursor = crHandPoint
    PopupMenu = PopupMenu
    OnMouseDown = GraphMouseDown
    OnMouseMove = GraphMouseMove
    OnMouseUp = GraphMouseUp
  end
  object pnlControl: TPanel
    Left = 0
    Top = 304
    Width = 673
    Height = 57
    TabOrder = 0
    OnResize = pnlControlResize
    DesignSize = (
      673
      57)
    object Label1: TLabel
      Left = 500
      Top = 8
      Width = 164
      Height = 13
      Caption = #1042#1088#1077#1084#1103' '#1086#1073#1085#1086#1074#1083#1077#1085#1080#1103'                  '#1084#1089
    end
    object btnStart: TButton
      Left = 600
      Top = 32
      Width = 25
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = '>'
      Default = True
      TabOrder = 0
      OnClick = btnStartClick
    end
    object btnPause: TButton
      Left = 624
      Top = 32
      Width = 25
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = '| |'
      TabOrder = 1
      OnClick = btnPauseClick
    end
    object btnStop: TButton
      Left = 648
      Top = 32
      Width = 25
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = '<|'
      TabOrder = 2
      OnClick = btnStopClick
    end
    object rbCurrent: TRadioButton
      Left = 40
      Top = 16
      Width = 41
      Height = 17
      Caption = #1058#1086#1082
      Checked = True
      TabOrder = 3
      TabStop = True
    end
    object rbVoltage: TRadioButton
      Left = 228
      Top = 16
      Width = 137
      Height = 17
      Caption = #1056#1072#1079#1085#1086#1089#1090#1100' '#1087#1086#1090#1077#1085#1094#1080#1072#1083#1086#1074
      TabOrder = 4
    end
    object cmbBranches: TComboBox
      Left = 40
      Top = 32
      Width = 170
      Height = 21
      BevelInner = bvNone
      BevelKind = bkTile
      ItemHeight = 13
      TabOrder = 5
      Text = '0'
    end
    object cmbPot1: TComboBox
      Left = 228
      Top = 32
      Width = 170
      Height = 21
      BevelInner = bvNone
      BevelKind = bkTile
      ItemHeight = 13
      TabOrder = 6
      Text = '1'
    end
    object cmbPot2: TComboBox
      Left = 404
      Top = 32
      Width = 170
      Height = 21
      BevelInner = bvNone
      BevelKind = bkTile
      ItemHeight = 13
      TabOrder = 7
      Text = '0'
    end
    object cmb_Timer_Value: TComboBox
      Left = 598
      Top = 5
      Width = 50
      Height = 21
      BevelEdges = []
      BevelInner = bvLowered
      BevelKind = bkFlat
      BevelOuter = bvNone
      Style = csDropDownList
      Color = clBtnFace
      DropDownCount = 10
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ItemIndex = 8
      ParentFont = False
      TabOrder = 8
      Text = '1000'
      OnChange = cmb_Timer_ValueChange
      Items.Strings = (
        '1'
        '10'
        '50'
        '100'
        '200'
        '300'
        '400'
        '500'
        '1000')
    end
  end
  object Scroll: TScrollBar
    Left = 0
    Top = 292
    Width = 669
    Height = 12
    PageSize = 0
    TabOrder = 1
    OnChange = ScrollChange
  end
  object Timer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = TimerTimer
    Left = 648
    Top = 8
  end
  object PopupMenu: TPopupMenu
    Left = 648
    Top = 32
    object mnu_Scale: TMenuItem
      Caption = #1052#1072#1089#1096#1090#1072#1073' '#1087#1086' Y'
      OnClick = mnu_ScaleClick
    end
    object Fit_Graph: TMenuItem
      Caption = #1042#1087#1080#1089#1072#1090#1100' '#1075#1088#1072#1092#1080#1082' '#1074' '#1086#1082#1085#1086
      OnClick = Fit_GraphClick
    end
    object Save: TMenuItem
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1075#1088#1072#1092#1080#1082
      OnClick = SaveClick
    end
  end
  object saveDlg: TSaveDialog
    DefaultExt = '.png'
    Left = 352
    Top = 200
  end
end
