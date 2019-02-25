//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainUnit.h"
#include <sstream>

extern "C"
{
  #include "UAServer.h"
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ReadButtonClick(TObject *Sender)
{
  std::string ip = AnsiString(IpEdit->Text).c_str();
  int port = PortEdit->Text.ToInt();
  uint16_t area = AreaEdit->Text.ToInt();
  uint16_t address = AddressEdit->Text.ToInt();
  int bitlen = BitlenEdit->Text.ToInt();
  ModbusClient.reset(new TMModbusTcpClient(ip,port,0));
  if (!ModbusClient->Connect())
  {
    ModbusClient.reset();
    throw Exception("Verbindungsaufbau fehlgeschlagen");
  }
  ModbusClient->ReadRequest(area,address,bitlen);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
  try {
    if (ModbusClient) {
      uint16_t area = AreaEdit->Text.ToInt();
      uint16_t address = AddressEdit->Text.ToInt();
      int bitlen = BitlenEdit->Text.ToInt();
      int bytelen = (bitlen+7)/8;
      std::unique_ptr<uint8_t[]> buffer(new uint8_t[bytelen]);
      int16_t result = ModbusClient->ReadResponse(area,address,bitlen,buffer.get());
      if (result==0) {
        // Puffer auslesen und darstellen
        std::ostringstream str;
        for (int i = 0; i < bytelen; i++) {
          str << std::hex << "0x" << (int)buffer[i] << " ,";
        }
        BufferMemo->Text = AnsiString(str.str().c_str());
        if (bytelen>=2) {
          // Zahlenwert des ersten Registers ausgeben
          Register1Edit->Text = IntToStr(buffer[0]<<8 | (buffer[1]));
        }
        if (ContiniousCheckBox->Checked)
        {
          // Daten erneut anfordern
          ModbusClient->ReadRequest(area,address,bitlen);
        }
        else
        {
          // Sonst die Verbindung wieder schlieﬂen
          ModbusClient.reset();
        }
      }
    }
  } catch (...) {
    ModbusClient.reset();
    throw;
  }
//  UAServer_loop();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StartUaButtonClick(TObject *Sender)
{
//  UAServer_start();
}
//---------------------------------------------------------------------------

