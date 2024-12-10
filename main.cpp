#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <locale>   // locale, setlocale
#include <fstream>  // ofstream, ifstream
#include <sstream>  // istringstream


using namespace std;

// Vakioiden m��rittely
const int MIN_HUONEIDEN_MAARA = 40;
const int MAX_HUONEIDEN_MAARA = 300;
const int YHDEN_HENGEN_HUONE_HINTA = 100;
const int KAHDEN_HENGEN_HUONE_HINTA = 150;
const int ALENNUS_PROSENTIT[] = { 0, 10, 20 }; // Alennusprosenttien valikoima

// Huoneen tiedot sis�lt�v� rakenne
struct Huone {
    int huonenumero;
    bool varattu;
    string varaajanNimi;
    int varausnumero;
    int hinta;
    string huonetyyppi; // Uusi kentt� huonetyypille ("1hh" tai "2hh")

    // Huoneen konstruktori
    Huone(int numero, int huoneHinta, string tyyppi) : huonenumero(numero), varattu(false), varaajanNimi(""), varausnumero(0), hinta(huoneHinta), huonetyyppi(tyyppi) {}
};

// Funktioiden esittely
void tulostaValikko();
void varaaHuone(vector<Huone>& huoneet, const string& huonetyyppi);
int arvoVapaaHuone(const vector<Huone>& huoneet, const string& huonetyyppi);
void arvoHuoneetJaHinnat(vector<Huone>& huoneet);
void naytaVapaatHuoneet(const vector<Huone>& huoneet);
void haeVarausNimella(const vector<Huone>& huoneet, const string& nimi);
void haeVarausNumerolla(const vector<Huone>& huoneet, int numero);
bool syotteenTarkistaminen(int syote, int ala, int yla);
int kysyKokonaisluku(string viesti, int ala, int yla);
void kayttajanValitsemaHuone(vector<Huone>& huoneet, const string& huonetyyppi);
void tallennaVaraukset(const vector<Huone>& huoneet, const string& tiedostonimi);
void lataaVaraukset(vector<Huone>& huoneet, const string& tiedostonimi);
void poistaVaraus(vector<Huone>& huoneet);
void tyhjennaKaikkiVaraukset(vector<Huone>& huoneet);

int main() {
    // Asetetaan suomenkielinen paikallisuus, jotta ��kk�set toimivat oikein
    setlocale(LC_ALL, "fi_FI");

    // Alustetaan satunnaislukugeneraattori
    srand(static_cast<unsigned int>(time(0)));

    // Luodaan ja alustetaan huoneet
    vector<Huone> huoneet;
    arvoHuoneetJaHinnat(huoneet);
    lataaVaraukset(huoneet, "varaukset.txt");

    bool jatka = true;
    while (jatka) {
        tulostaValikko();
        int valinta = kysyKokonaisluku("Valintasi: ", 1, 7);

        switch (valinta) {
        case 1: {
            cout << "Valitse huonetyyppi: 1. Yhden hengen huone (1hh), 2. Kahden hengen huone (2hh)" << endl;
            int tyyppiValinta = kysyKokonaisluku("Valintasi: ", 1, 2);
            string huonetyyppi = (tyyppiValinta == 1) ? "1hh" : "2hh";

            cout << "Haluatko valita huoneen itse? 1. Kyll�, 2. Ei (j�rjestelm� valitsee vapaan huoneen)" << endl;
            int valinta = kysyKokonaisluku("Valintasi: ", 1, 2);

            if (valinta == 1) {
                kayttajanValitsemaHuone(huoneet, huonetyyppi);
            }
            else {
                varaaHuone(huoneet, huonetyyppi);
            }
            break;
        }
        case 2:
            naytaVapaatHuoneet(huoneet);
            break;
        case 3: {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string nimi;
            cout << "Anna varaajan nimi: ";
            getline(cin, nimi);
            haeVarausNimella(huoneet, nimi);
            break;
        }
        case 4: {
            int varausnumero = kysyKokonaisluku("Anna varausnumero: ", 10000, 99999);
            haeVarausNumerolla(huoneet, varausnumero);
            break;
        }
        case 5:
            poistaVaraus(huoneet);
            break;
        case 6:
            tyhjennaKaikkiVaraukset(huoneet);
            cout << "Kaikki varaukset on tyhjennetty." << endl;
            break;
        case 7:
            tallennaVaraukset(huoneet, "varaukset.txt");
            cout << "Kiitos k�yt�st�!" << endl;
            jatka = false;
            break;
        default:
            cout << "Virheellinen valinta, yrit� uudelleen." << endl;
            break;
        }
    }

    return 0;
}

// Valikon tulostaminen
// Tulostaa p��valikon k�ytt�j�lle
void tulostaValikko() {
    cout << "\nValitse toiminto:" << endl;
    cout << "1. Varaa huone" << endl;
    cout << "2. N�yt� vapaat huoneet" << endl;
    cout << "3. Hae varaus varaajan nimell�" << endl;
    cout << "4. Hae varaus varausnumerolla" << endl;
    cout << "5. Poista varaus" << endl;
    cout << "6. Tyhjenn� kaikki varaukset" << endl;
    cout << "7. Poistu ohjelmasta (ja tallenna varaukset)" << endl;
}

// Varausten poistaminen
void poistaVaraus(vector<Huone>& huoneet) {
    int varausnumero = kysyKokonaisluku("Anna poistettavan varauksen numero: ", 10000, 99999);
    for (auto& huone : huoneet) {
        if (huone.varattu && huone.varausnumero == varausnumero) {
            huone.varattu = false;
            huone.varaajanNimi = "";
            huone.varausnumero = 0;
            cout << "Varaus numero " << varausnumero << " poistettu onnistuneesti." << endl;
            return;
        }
    }
    cout << "Varausta numerolla " << varausnumero << " ei l�ytynyt." << endl;
}

// Kaikkien varausten tyhjent�minen
void tyhjennaKaikkiVaraukset(vector<Huone>& huoneet) {
    for (auto& huone : huoneet) {
        huone.varattu = false;
        huone.varaajanNimi = "";
        huone.varausnumero = 0;
    }
}

// Huoneen varaaminen
void varaaHuone(vector<Huone>& huoneet, const string& huonetyyppi) {
    int huonenumero = arvoVapaaHuone(huoneet, huonetyyppi);

    if (huonenumero == -1) {
        cout << "Kaikki valitsemasi tyypin huoneet on varattu." << endl;
        return;
    }

    int oidenMaara = kysyKokonaisluku("Anna �iden m��r�: ", 1, 100);
    int alennusProsentti = ALENNUS_PROSENTIT[rand() % 3];
    int alkuperainenSumma = oidenMaara * huoneet[huonenumero - 1].hinta;
    int loppusumma = alkuperainenSumma * (100 - alennusProsentti) / 100;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string varaajanNimi;
    cout << "Anna varaajan nimi: ";
    getline(cin, varaajanNimi);

    int varausnumero = rand() % 90000 + 10000;

    huoneet[huonenumero - 1].varattu = true;
    huoneet[huonenumero - 1].varaajanNimi = varaajanNimi;
    huoneet[huonenumero - 1].varausnumero = varausnumero;

    cout << "Varaus onnistui! Loppusumma: " << loppusumma << " euroa (" << alennusProsentti << "% alennus)" << endl;
    cout << "Varausnumero: " << varausnumero << endl;
    cout << "Huonetyyppi: " << huoneet[huonenumero - 1].huonetyyppi << endl;
}

// Vapaan huoneen arpominen
int arvoVapaaHuone(const vector<Huone>& huoneet, const string& huonetyyppi) {
    vector<int> vapaatHuoneet;
    for (const auto& huone : huoneet) {
        if (!huone.varattu && huone.huonetyyppi == huonetyyppi) {
            vapaatHuoneet.push_back(huone.huonenumero);
        }
    }

    if (vapaatHuoneet.empty()) {
        return -1;
    }

    int satunnainenIndeksi = rand() % static_cast<int>(vapaatHuoneet.size());
    return vapaatHuoneet[satunnainenIndeksi];
}

// K�ytt�j�n valitsema huone
void kayttajanValitsemaHuone(vector<Huone>& huoneet, const string& huonetyyppi) {
    naytaVapaatHuoneet(huoneet);
    int huonenumero = kysyKokonaisluku("Anna varattavan huoneen numero: ", 1, static_cast<int>(huoneet.size()));

    if (huonenumero < 1 || huonenumero > huoneet.size() || huoneet[huonenumero - 1].varattu || huoneet[huonenumero - 1].huonetyyppi != huonetyyppi) {
        cout << "Huone ei ole varattavissa tai huonetyyppi ei t�sm��. Yrit� uudelleen." << endl;
        return;
    }

    int oidenMaara = kysyKokonaisluku("Anna �iden m��r�: ", 1, 100);
    int alennusProsentti = ALENNUS_PROSENTIT[rand() % 3];
    int alkuperainenSumma = oidenMaara * huoneet[huonenumero - 1].hinta;
    int loppusumma = alkuperainenSumma * (100 - alennusProsentti) / 100;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string varaajanNimi;
    cout << "Anna varaajan nimi: ";
    getline(cin, varaajanNimi);

    int varausnumero = rand() % 90000 + 10000;

    huoneet[huonenumero - 1].varattu = true;
    huoneet[huonenumero - 1].varaajanNimi = varaajanNimi;
    huoneet[huonenumero - 1].varausnumero = varausnumero;

    cout << "Varaus onnistui! Loppusumma: " << loppusumma << " euroa (" << alennusProsentti << "% alennus)" << endl;
    cout << "Varausnumero: " << varausnumero << endl;
    cout << "Huonetyyppi: " << huoneet[huonenumero - 1].huonetyyppi << endl;
}

// Huoneiden ja hintojen arpominen
void arvoHuoneetJaHinnat(vector<Huone>& huoneet) {
    int huoneidenMaara = rand() % (MAX_HUONEIDEN_MAARA - MIN_HUONEIDEN_MAARA + 1) + MIN_HUONEIDEN_MAARA;
    int yhdenHengenMaara = huoneidenMaara / 2;
    int kahdenHengenMaara = huoneidenMaara - yhdenHengenMaara;

    for (int i = 0; i < yhdenHengenMaara; ++i) {
        huoneet.emplace_back(i + 1, YHDEN_HENGEN_HUONE_HINTA, "1hh");
    }

    for (int i = yhdenHengenMaara; i < huoneidenMaara; ++i) {
        huoneet.emplace_back(i + 1, KAHDEN_HENGEN_HUONE_HINTA, "2hh");
    }
}

// Vapaat huoneet n�ytt�v� funktio
void naytaVapaatHuoneet(const vector<Huone>& huoneet) {
    cout << "\nVapaat huoneet:" << endl;
    bool onVapaita = false;
    for (const auto& huone : huoneet) {
        if (!huone.varattu) {
            cout << "Huone " << huone.huonenumero << " (" << huone.huonetyyppi << "), hinta: " << huone.hinta << " euroa/y�" << endl;
            onVapaita = true;
        }
    }
    if (!onVapaita) {
        cout << "Ei vapaita huoneita." << endl;
    }
}

// Varausten haku varaajan nimell�
void haeVarausNimella(const vector<Huone>& huoneet, const string& nimi) {
    cout << "\nHaetaan varauksia nimell�: " << nimi << endl;
    bool loytyi = false;
    for (const auto& huone : huoneet) {
        if (huone.varattu && huone.varaajanNimi == nimi) {
            cout << "Huone " << huone.huonenumero << " (" << huone.huonetyyppi << "), varausnumero: " << huone.varausnumero << ", hinta: " << huone.hinta << " euroa/y�" << endl;
            loytyi = true;
        }
    }
    if (!loytyi) {
        cout << "Ei l�ytynyt varauksia annetulla nimell�." << endl;
    }
}

// Varausten haku varausnumerolla
void haeVarausNumerolla(const vector<Huone>& huoneet, int numero) {
    cout << "\nHaetaan varausta numerolla: " << numero << endl;
    for (const auto& huone : huoneet) {
        if (huone.varattu && huone.varausnumero == numero) {
            cout << "Huone " << huone.huonenumero << " (" << huone.huonetyyppi << "), varaajan nimi: " << huone.varaajanNimi << ", hinta: " << huone.hinta << " euroa/y�" << endl;
            return;
        }
    }
    cout << "Ei l�ytynyt varausta annetulla varausnumerolla." << endl;
}

// Sy�tteen tarkistaminen ja kokonaisluvun kysyminen
bool syotteenTarkistaminen(int syote, int ala, int yla) {
    return (syote >= ala && syote <= yla);
}

int kysyKokonaisluku(string viesti, int ala, int yla) {
    int syote;
    while (true) {
        cout << viesti;
        cin >> syote;
        if (cin.fail() || !syotteenTarkistaminen(syote, ala, yla)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Virheellinen sy�te, yrit� uudelleen." << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return syote;
        }
    }
}

// Varausten tallentaminen tiedostoon
void tallennaVaraukset(const vector<Huone>& huoneet, const string& tiedostonimi) {
    ofstream tiedosto(tiedostonimi, ios::trunc);
    if (!tiedosto) {
        cerr << "Virhe: varauksia ei voitu tallentaa tiedostoon." << endl;
        return;
    }

    for (const auto& huone : huoneet) {
        tiedosto << huone.huonenumero << '|'
            << huone.varattu << '|'
            << '"' << huone.varaajanNimi << '"' << '|'
            << huone.varausnumero << '|'
            << huone.hinta << '|'
            << '"' << huone.huonetyyppi << '"' << '\n';
    }
    cout << "Varaukset tallennettu tiedostoon: " << tiedostonimi << endl;
}

// Varausten lataaminen tiedostosta
void lataaVaraukset(vector<Huone>& huoneet, const string& tiedostonimi) {
    ifstream tiedosto(tiedostonimi);
    if (!tiedosto) {
        cout << "Varauksia ei l�ytynyt ladattavaksi." << endl;
        return;
    }

    huoneet.clear();
    string rivi;
    while (getline(tiedosto, rivi)) {
        istringstream iss(rivi);
        string varaajanNimi, huonetyyppi;
        int huonenumero, varausnumero, hinta;
        bool varattu;
        char erotin; // '|'

        if (iss >> huonenumero >> erotin >> varattu >> erotin &&
            getline(iss, varaajanNimi, '|') &&
            iss >> varausnumero >> erotin >> hinta >> erotin &&
            getline(iss, huonetyyppi, '|')) {

            // Poista lainausmerkit varaajanNimesta ja huonetyypist�
            varaajanNimi.erase(remove(varaajanNimi.begin(), varaajanNimi.end(), '"'), varaajanNimi.end());
            huonetyyppi.erase(remove(huonetyyppi.begin(), huonetyyppi.end(), '"'), huonetyyppi.end());

            huoneet.emplace_back(huonenumero, hinta, huonetyyppi);
            huoneet.back().varattu = varattu;
            huoneet.back().varaajanNimi = varaajanNimi;
            huoneet.back().varausnumero = varausnumero;
        }
    }
    cout << "Varaukset ladattu tiedostosta: " << tiedostonimi << endl;
}

