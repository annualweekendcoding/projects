object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'ModbusTest'
  ClientHeight = 351
  ClientWidth = 553
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 56
    Top = 48
    Width = 53
    Height = 13
    Caption = 'IP-Adresse'
  end
  object Label2: TLabel
    Left = 56
    Top = 80
    Width = 20
    Height = 13
    Caption = 'Port'
  end
  object Label3: TLabel
    Left = 56
    Top = 120
    Width = 35
    Height = 13
    Caption = 'Bereich'
  end
  object Label4: TLabel
    Left = 56
    Top = 152
    Width = 39
    Height = 13
    Caption = 'Adresse'
  end
  object Label5: TLabel
    Left = 56
    Top = 184
    Width = 45
    Height = 13
    Caption = 'Bit-L'#228'nge'
  end
  object Label6: TLabel
    Left = 56
    Top = 229
    Width = 29
    Height = 13
    Caption = 'Daten'
  end
  object ReadButton: TButton
    Left = 312
    Top = 131
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 0
    OnClick = ReadButtonClick
  end
  object WriteButton: TButton
    Left = 312
    Top = 178
    Width = 75
    Height = 25
    Caption = 'Write'
    TabOrder = 1
  end
  object IpEdit: TEdit
    Left = 120
    Top = 45
    Width = 121
    Height = 21
    TabOrder = 2
    Text = '192.168.179.199'
  end
  object PortEdit: TEdit
    Left = 120
    Top = 77
    Width = 121
    Height = 21
    TabOrder = 3
    Text = '502'
  end
  object AreaEdit: TEdit
    Left = 120
    Top = 117
    Width = 121
    Height = 21
    TabOrder = 4
    Text = '4'
    TextHint = '0,1,3,4'
  end
  object AddressEdit: TEdit
    Left = 120
    Top = 149
    Width = 121
    Height = 21
    TabOrder = 5
    Text = '0'
    TextHint = '0-65535'
  end
  object BitlenEdit: TEdit
    Left = 120
    Top = 181
    Width = 121
    Height = 21
    TabOrder = 6
    Text = '16'
    TextHint = 'L'#228'nge in Bit'
  end
  object BufferMemo: TMemo
    Left = 120
    Top = 226
    Width = 409
    Height = 55
    TabOrder = 7
  end
  object ContiniousCheckBox: TCheckBox
    Left = 408
    Top = 135
    Width = 97
    Height = 17
    Caption = 'Fortlaufend'
    TabOrder = 8
  end
  object Register1Edit: TEdit
    Left = 120
    Top = 287
    Width = 73
    Height = 21
    ReadOnly = True
    TabOrder = 9
  end
  object StartUaButton: TButton
    Left = 312
    Top = 24
    Width = 105
    Height = 25
    Caption = 'Start UA-Server'
    TabOrder = 10
    OnClick = StartUaButtonClick
  end
  object Timer1: TTimer
    Interval = 100
    OnTimer = Timer1Timer
    Left = 480
    Top = 24
  end
end
