- Installazione DsOdoo

1) Estrarre il contenuto dell'archivio su una cartella locale (Es. Desktop Utente)
2) Entrare nella cartella backend\installer
3) Eseguire come Amministratore (tasto destro, esegui come amministratore) il file: install_run_as_administrator.bat e iniziare l'installazione premendo invio.
3a) Qualora il software OpenSc sia già stato installato, chiudere la sua finestra senza mandare avanti l'installazione di OpenSc e proseguire.
3b) Mandare avanti con: Invio tutti i passi richiesti e consentire a: install_run_as_administrator.bat di apportare eventuali modifiche al registro di sistema.
4) Al termine la finestra si chiuderà e l'installazione sarà completata.


- Installazione CHIAVI USB ARUBA

E' necessario installare il software: AK Switcher Service
1) Il software è reperibile all'interno della Chiave USB nella cartella utility, installarlo mandando avanti la procedura.


- Modifiche Nuove CHIAVI USB ARUBA (con Windows 32/64Bit)

Per quanto riguarda le nuove chiavi usb di Aruba con chiave a 2048bit, se il software DsOdoo non dovesse riconoscerle (Icona rossa anche con chiave installata correttamente) 
esse hanno la necessità di una modifica sul file: c:\usr\local\ssl\get_certificate.bat

1) Nella cartella utility della Chiave USB Aruba, installare il pacchetto relativo a bit4id mandando avanti la sua installazione.
2) Accertarsi che nella cartella di sistema di Windows (C:\Windows\SysWow64 (64bit) o C:\Windows\System32 (32bit)) si presente un file dll denominato: bit4ipki.dll
3) Portarsi nella cartella c:\usr\local\ssl e col tasto destro sul file: get_certificate.bat premere "modifica"
4) Trovare la riga contenente la seguente stringa: /usr/local/ssl/lib/bit4ipki.dll
5) Sostituire questo testo: /usr/local/ssl/lib/bit4ipki.dll con /Windows/SysWow64/bit4ipki.dll nel caso di Windows a 64Bit oppure /Windows/System32/bit4ipki.dll nel caso di Windows a 32bit.
4) Salvare il file e verificare il riconoscimento della chiave da parte di DsOdoo (Icona verde)