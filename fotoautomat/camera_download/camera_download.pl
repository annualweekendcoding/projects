#!/usr/bin/perl

use strict;

my $cameraNetwork='192.168.0';
my $cameraUrl="http://$cameraNetwork.1";
my $cameraDir='camera/';
my $cameraReadyDir='camera_ready/';

my $ftp_uri="";

# zuerst auf IP-Adresse warten (Netzwerkverbindung zur Kamera)
# dann Dateien anfordern
# dann jeweils bei Datei-Event Dateien anfordern
# Wenn Verbindung abbricht, dann erneut IP-Adresse ermitteln

while (1)
{
  my $ok=0;
  if ($ftp_uri ne "")
  {
    open CURL, "curl -s '$cameraUrl/command.cgi?op=102' |";
    while (<CURL>)
    {
      chomp();
      if ($_ eq "1")
      {
        auto_upload();
        $ok=1;
      }  
      elsif ($_ eq "0")
      {
        $ok=1;
      }  
    }
    close CURL;
  }
  if (! $ok)
  {    
    wait_for_ip();
    auto_upload();
  }   
  sleep(1);
}

sub auto_upload()
{
  open CURL, "curl -s '$cameraUrl/lua/auto_upload.lua?ftp_uri=$ftp_uri&singlefile=0' |";
  while (<CURL>)
  {
    print $_;
    # beim Laden die fertigen Dateien schon mal verschieben
    my ($file) = (/^STORED (.*)$/);
    if ($file ne "")
    {
      system ("mv $cameraDir$file $cameraReadyDir"); 
    }
  }
  # nach dem Laden die restlichen Dateien verschieben
  my @a = glob("$cameraDir*");
  if ($#a >=0)
  {
    system ("mv $cameraDir* $cameraReadyDir"); 
  }
}     

sub wait_for_ip()
{
  # warten bis wir eine gültige IP-Adresse im Kameranetzwerk haben
  while ($ftp_uri eq "")
  {
    open IFCONFIG, "ifconfig | grep $cameraNetwork |";
    while (<IFCONFIG>)
    {
      my ($ipaddress) = (/inet .*($cameraNetwork\.\d+).*$cameraNetwork.*/);
      $ftp_uri = "ftp://pi:.pi\@$ipaddress/$cameraDir" if ($ipaddress ne "");
    }
    sleep(1) if ($ftp_uri eq "");
  }
  print "Adresse ermittelt: $ftp_uri\n";  
}

