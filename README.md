# Kwidzyński Mariusz Paweł

## Wstęp

W klasie `Slavs` są:

- prywatne zmienne od statystyk(_id/_name, _lv, _hp, _vit, _str, _dex, _int, _agi, _exp) _m_per, _per, _inv[3], _x, _y
- funkcje decide(), advance_decide(), skill_assign(), defend(), magic(),
- publiczne attack() i stats();

_per(0.01 - 0.99) to seed 'osobowosci dla funkcji pseudolosowej x = 4 * x_prev * (1 - x_prev), wszedie kozystam z advance_decision() wiec chyba usune decision().

_m_per(1-4) to obrót('obrazek' ponizej) dla advance_decide() kozystajacej z x = 60 * x_prev * cos(x_prev - y_prev), y = 60 * y_prev * cos(y_prev - x_prev) i tworzy 'stozek' na polu 100x100 ktorym mozna obracac o 90 stopni mnozac odpowiednia wspólzedna razy -1 i dodajac 99

skill_asign() przydiela punkty na podstawie advance_decide(),
w attack(Slavs *atakowany) jest losowany ruch attack, defend(), magic(), evade a potem jest wywolywane atakowany->defend() i wybierany jest ruch dla broniacego sie nie wymyslilem jeszcze jak to bedzie z magia, myslalem o czyms prostym jak ogien, lod, leczenie.

W _inv[3] jest przechowywane id broni, tarczy, zbroi (zawsze mozna to zmienic na helm, zbroje, buty itp.), myslalem o jakiejs publicznej mapie z id i struktura parameters(power, chance, lv)
stats() zwraca po prostu char* z kompletnym opisem, sa tez gettery do statystyk.

(przygotowalem to sobie wczesniej i juz mi sie nie chce tego takos przpisywac)

	1        rational       2
	   1                     2
	      1               2
	         1          2
	            1    2
	introvert 0 extrovert
	            4    3
	         4         3
	      4              3
	   4                    3
	4     emotional     3

w zaleznosci od procentowych udzialów danych osobowosci przy przydielaniu statystyk  uwzgledniane sa predyspozycje:

- passive 
- introvert +def
- extrovert +str
- rational  +int
- emotional +agi

podczas walki sa tez bonusy w zaleznosci od zlozenia osobowosci o najwiekszym udziale (_m_per)

- active
- 1
	- analyze - analyze enemy's next move and choose best 	- counterattack
	- precision - +15% chance to attack/defend
- 2
	- determined - 10% chance of evading deadly attack
	- ambitious -     +10% dmg
- 3
	- mobile -        +15% chance to evade
	- impulsive -     10% chance to surprise attack (enemy cant def if enemy.agi < agi)
- 4
	- patient -       defend till enemy drop guard, +10%dmg when attacking
                +10% def

## Opis działania

Losowana jest jakas ilosc osób i umieszczana na polu odpowiedniej wielkosci a potem dla kazdego wybierana jest akcja ruchu lub przeszukiwań.

Ekstrowertycy poruszaja sie w strone innych introwertycy unikaja innych, racjonalni zwracaja uwage na szanse na zwyciestwo itp.
i co jakis czas sa losowane punkty w ktorych mozna znalezc lepszy ekwipunek, ogolnie nikt nie ma zadnych przedmiotow, ale mozna ich szukac w kazdym miejscu.

W wylosowanych punktach jest po prostu wieksza szansa na lepsze przedmioty, im ma sie gorsze przedmioty tym wieksza szansa na znalezienia ( 1 - item.lv / player.lv).

Mozna by tez pozniej dodac przeciwnikow zwierzeta itp. zeby nie musieli sie bic ze soba zeby zwiekszac poziom.
