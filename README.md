# IPK Projekt 1
## vytvorenie servera v jazyku C/C++

## Build
Pred prvým spustením programu je nutné prejsť do adresára, kde sú cieľové skripty uložené a 
za pomoci príkazu make preložiť skripty.

```bash
make 
```

## Spustenie programu

Pri spustení programu využívame nasledujúci príkaz:

```bash
./hinfosvc [číslo portu]
```
Po spustení sa zobrazí v príkazovom riadku url adresa cez ktorú pristupíme k serveru.

## Príklad použitia
Príklad spustenia serveru na porte 8080:

```bash
./hinfosvc 8080
```

## Funkcionalita
Po úspešnom spustení programu sa uvedie do funkčnosti server. V príklade využívame port 8080.
Následne je možné prejsť do webového prehliadača a zadať url adresu na základe úlohy, ktorú vyžadujeme vykonať.
Pokiaľ zadáme nesprávnu url adresu, v prehliadači sa zobrazí "400 Bad Request".

Zobrazenie doménového názvu zariadenia.
```
http://localhost:8080/hostname
```

Zobrazenie názvu procesora serveru.
```
http://localhost:8080/cpu-name
```

Zobrazenie využitia procesora serveru.
```
http://localhost:8080/load
```

