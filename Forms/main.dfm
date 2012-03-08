object frmMain: TfrmMain
  Left = 185
  Top = 196
  Width = 913
  Height = 478
  Caption = 'Power System'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnDestroy = FormDestroy
  OnDragDrop = FormDragDrop
  OnDragOver = FormDragOver
  OnMouseDown = FormMouseDown
  OnPaint = FormPaint
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object lblSec: TLabel
    Left = 800
    Top = 8
    Width = 21
    Height = 13
    Caption = #1089#1077#1082'.'
  end
  object lblTime: TStaticText
    Left = 760
    Top = 8
    Width = 40
    Height = 20
    Hint = #1058#1077#1082#1091#1097#1077#1077' '#1074#1088#1077#1084#1103
    AutoSize = False
    Caption = '0'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
  end
  object btnStart: TButton
    Left = 824
    Top = 0
    Width = 25
    Height = 25
    Hint = #1057#1090#1072#1088#1090
    Caption = '>'
    Default = True
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = btnStartClick
  end
  object btnPause: TButton
    Left = 848
    Top = 0
    Width = 25
    Height = 25
    Hint = #1055#1072#1091#1079#1072
    Caption = '| |'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = btnPauseClick
  end
  object btnStop: TButton
    Left = 872
    Top = 0
    Width = 25
    Height = 25
    Hint = #1055#1088#1080#1085#1103#1090#1100' '#1101#1090#1086#1090' '#1084#1086#1084#1077#1085#1090' '#1074#1088#1077#1084#1077#1085#1080' '#1079#1072' 0'
    Caption = '< |'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = btnStopClick
  end
  object MainMenu1: TMainMenu
    Left = 32
    object mnuFile: TMenuItem
      Caption = #1060#1072#1081#1083
      object mnuNew: TMenuItem
        Caption = #1057#1086#1079#1076#1072#1090#1100' '#1087#1088#1086#1077#1082#1090
        OnClick = mnuNewClick
      end
      object mnuSave: TMenuItem
        Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
        OnClick = mnuSaveClick
      end
      object mnuOpen: TMenuItem
        Caption = #1054#1090#1082#1088#1099#1090#1100
        OnClick = mnuOpenClick
      end
    end
    object mnuTools: TMenuItem
      Caption = #1048#1085#1089#1090#1088#1091#1084#1077#1085#1090#1099
      object BuildGrid: TMenuItem
        Caption = #1057#1073#1086#1088#1082#1072' '#1089#1077#1090#1080
        OnClick = BuildGridClick
      end
      object mnuObjLib: TMenuItem
        Caption = #1041#1080#1073#1083#1080#1086#1090#1077#1082#1072' '#1086#1073#1098#1077#1082#1090#1086#1074
        OnClick = mnuObjLibClick
      end
      object mnuScope: TMenuItem
        Caption = #1054#1089#1094#1080#1083#1086#1075#1088#1072#1092
        Enabled = False
        OnClick = mnuScopeClick
      end
    end
    object mnuHelp: TMenuItem
      Caption = #1057#1087#1088#1072#1074#1082#1072
      object mnuAbout: TMenuItem
        Caption = #1054' '#1087#1088#1086#1075#1088#1072#1084#1084#1077
        OnClick = mnuAboutClick
      end
    end
  end
  object Timer: TTimer
    Enabled = False
    Interval = 1
    OnTimer = TimerTimer
  end
  object saveDlg: TSaveDialog
    DefaultExt = '.png'
    Left = 64
  end
  object openDlg: TOpenDialog
    Left = 96
  end
end
