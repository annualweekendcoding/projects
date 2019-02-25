//---------------------------------------------------------------------------

#ifndef MainUnitH
#define MainUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <memory>
#include "MModbusTcpClient.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// Von der IDE verwaltete Komponenten
	TButton *ReadButton;
	TButton *WriteButton;
	TLabel *Label1;
	TEdit *IpEdit;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TEdit *PortEdit;
	TEdit *AreaEdit;
	TEdit *AddressEdit;
	TEdit *BitlenEdit;
	TMemo *BufferMemo;
	TLabel *Label6;
	TTimer *Timer1;
	TCheckBox *ContiniousCheckBox;
	TEdit *Register1Edit;
	TButton *StartUaButton;
	void __fastcall ReadButtonClick(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall StartUaButtonClick(TObject *Sender);
private:	// Benutzer-Deklarationen
  std::unique_ptr<TMModbusTcpClient> ModbusClient;
public:		// Benutzer-Deklarationen
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
