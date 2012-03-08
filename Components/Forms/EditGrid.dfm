object frmEditGrid: TfrmEditGrid
  Left = 239
  Top = 162
  Width = 860
  Height = 445
  Caption = #1056#1077#1076#1072#1082#1090#1080#1088#1086#1074#1072#1090#1100' '#1089#1077#1090#1100
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  OnClose = FormClose
  OnDragDrop = FormDragDrop
  OnDragOver = FormDragOver
  OnMouseDown = FormMouseDown
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object saveDlg: TSaveDialog
    DefaultExt = '.png'
    Left = 40
  end
  object openDlg: TOpenDialog
    Left = 72
  end
  object MainMenu1: TMainMenu
    Left = 8
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
        Visible = False
      end
      object mnuObjLib: TMenuItem
        Caption = #1041#1080#1073#1083#1080#1086#1090#1077#1082#1072' '#1086#1073#1098#1077#1082#1090#1086#1074
        OnClick = mnuObjLibClick
      end
    end
  end
end
