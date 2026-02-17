#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

using namespace std;

//Stałe
const int MAX = 52; //gra na jedną talie

// ------
// GAMELOOP
// ------

void Initialize();
void Update();
void Render();
void Shutdown();
void Showdown();

// ------
// KARTA
// ------

class card {
public:
	char value; //wartość 2,3,4,5,6,7,8,9,T,J,Q,K,A
	string suit; //kolory

	void printcard()
	{
		cout << value << " of " << suit;
	}

};
card deck[MAX], temp;

int top = 0;

card DrawCard()
{
	return deck[top++]; // zwraca następną kartę i przesuwa top
}

// ------
// GRACZ
// ------

class Player {
public:
	string name;
	card hand[2]; //reka startowa
	int cardsInHand;

	Player(string n = "Player") {
		name = n;
		cardsInHand = 0;
	}

	void resetHand() {
		cardsInHand = 0;
	}

	void takeCard(card c) {
		if (cardsInHand < 2) {
			hand[cardsInHand] = c;
			cardsInHand++;
		}
	}

	void printHand(bool hide = false) { //nie pokazuje kart przeciwnika
		cout << name << " hand: ";
		if (hide) {
			cout << "[***], [***]\n";
			return;
		}
		for (int i = 0; i < cardsInHand; i++) {
			hand[i].printcard();
			if (i < cardsInHand - 1) cout << ", ";
		}
		cout << endl;
	}
};
Player player1("Player 1");
Player PC("COMPUTER");

// ------
// PLANSZA
// ------
card flop1, flop2, flop3, turnCard, riverCard;
bool flopDealt = 0;
bool turnDealt = 0;
bool riverDealt = 0;

// ----------
// ROZGRYWKA
// ----------

bool IS_GAME_FINISHED = 0;
bool quitProgram = 0;
bool playerDropped = 0;
int run = 0; // 0 preflop, 1 flop, 2 turn, 3 river, 4 showdown

// =====================================================
// Ewaluator: best 5 z 7 kart
// =====================================================

enum HandCategory {
    HIGH_CARD = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR,
    STRAIGHT_FLUSH
};

struct HandValue {
    HandCategory cat = HIGH_CARD;
    array<int, 5> tie{}; // tie-breakery
};

static bool operator<(const HandValue& a, const HandValue& b) {
    if (a.cat != b.cat) return a.cat < b.cat;
    return a.tie < b.tie;
}

static int RankCharToInt(char v) {
    if (v >= '2' && v <= '9') return v - '0';
    switch (v) {
    case 'T': return 10;
    case 'J': return 11;
    case 'Q': return 12;
    case 'K': return 13;
    case 'A': return 14;
    default:  return 0;
    }
}

static int SuitToInt(const string& s) {
    if (s == "Hearts")   return 0;
    if (s == "Diamonds") return 1;
    if (s == "Clubs")    return 2;
    if (s == "Spades")   return 3;
    return -1;
}

static string CategoryName(HandCategory c) {
    switch (c) {
    case HIGH_CARD:      return "High Card";
    case ONE_PAIR:       return "One Pair";
    case TWO_PAIR:       return "Two Pair";
    case THREE:          return "Three of a Kind";
    case STRAIGHT:       return "Straight";
    case FLUSH:          return "Flush";
    case FULL_HOUSE:     return "Full House";
    case FOUR:          return "Four of a Kind";
    case STRAIGHT_FLUSH: return "Straight Flush";
    default:             return "Unknown";
    }
}

static HandValue Evaluate5(const array<card, 5>& h) {
    array<int, 5> r{};
    array<int, 5> su{};
    for (int i = 0; i < 5; i++) {
        r[i] = RankCharToInt(h[i].value);
        su[i] = SuitToInt(h[i].suit);
    }

    // Czy flush? - Wszystkie karty mają ten sam kolor
    bool flush = true;
    for (int i = 1; i < 5; i++) {
        if (su[i] != su[0]) { flush = false; break; }
    }

    vector<int> ranks(r.begin(), r.end());
    sort(ranks.begin(), ranks.end(), greater<int>());

    // Czy straight? (A-5)  - Karty w kolejności
    bool straight = false;
    int straightHigh = 0;
    {
        vector<int> u = ranks;
        u.erase(unique(u.begin(), u.end()), u.end());
        if (u.size() == 5) {
            if (u[0] - u[4] == 4) {
                straight = true;
                straightHigh = u[0];
            }
            else {
                vector<int> wheel{ 14, 5, 4, 3, 2 };
                if (u == wheel) {
                    straight = true;
                    straightHigh = 5;
                }
            }
        }
    }

    array<int, 15> cnt{};
    for (int x : ranks) cnt[x]++;

    vector<pair<int, int>> groups;
    for (int rr = 14; rr >= 2; rr--) {
        if (cnt[rr]) groups.push_back({ cnt[rr], rr });
    }
    sort(groups.begin(), groups.end(), [](auto a, auto b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second > b.second;
        });

    HandValue hv;
    hv.tie = { 0,0,0,0,0 };

    if (straight && flush) {
        hv.cat = STRAIGHT_FLUSH;
        hv.tie[0] = straightHigh;
        return hv;
    }

    if (groups[0].first == 4) {
        hv.cat = FOUR;
        hv.tie[0] = groups[0].second;
        for (int rr = 14; rr >= 2; rr--) if (cnt[rr] == 1) { hv.tie[1] = rr; break; }
        return hv;
    }

    if (groups[0].first == 3 && groups.size() > 1 && groups[1].first == 2) {
        hv.cat = FULL_HOUSE;
        hv.tie[0] = groups[0].second;
        hv.tie[1] = groups[1].second;
        return hv;
    }

    if (flush) {
        hv.cat = FLUSH;
        for (int i = 0; i < 5; i++) hv.tie[i] = ranks[i];
        return hv;
    }

    if (straight) {
        hv.cat = STRAIGHT;
        hv.tie[0] = straightHigh;
        return hv;
    }

    if (groups[0].first == 3) {
        hv.cat = THREE;
        hv.tie[0] = groups[0].second;
        int t = 1;
        for (int rr = 14; rr >= 2; rr--) if (cnt[rr] == 1) hv.tie[t++] = rr;
        return hv;
    }

    if (groups[0].first == 2 && groups.size() > 1 && groups[1].first == 2) {
        hv.cat = TWO_PAIR;
        hv.tie[0] = groups[0].second;
        hv.tie[1] = groups[1].second;
        for (int rr = 14; rr >= 2; rr--) if (cnt[rr] == 1) { hv.tie[2] = rr; break; }
        return hv;
    }

    if (groups[0].first == 2) {
        hv.cat = ONE_PAIR;
        hv.tie[0] = groups[0].second;
        int t = 1;
        for (int rr = 14; rr >= 2; rr--) if (cnt[rr] == 1) hv.tie[t++] = rr;
        return hv;
    }

    hv.cat = HIGH_CARD;
    for (int i = 0; i < 5; i++) hv.tie[i] = ranks[i];
    return hv;
}

static HandValue BestFrom(const vector<card>& cards) {
    int n = (int)cards.size();
    array<card, 5> h{};
    HandValue best;
    bool first = true;

    for (int a = 0; a < n - 4; a++)
        for (int b = a + 1; b < n - 3; b++)
            for (int c = b + 1; c < n - 2; c++)
                for (int d = c + 1; d < n - 1; d++)
                    for (int e = d + 1; e < n; e++) {
                        h = { cards[a], cards[b], cards[c], cards[d], cards[e] };
                        HandValue cur = Evaluate5(h);
                        if (first || best < cur) { best = cur; first = false; }
                    }
    return best;
}

// ----------
// TALIA
// ----------

void BuildDeck() {
    char value[]{ '2','3','4','5','6','7','8','9','T','J','Q','K','A' };
    string suit[]{ "Hearts","Diamonds","Clubs","Spades" };

    int l = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            deck[l].value = value[j];
            deck[l].suit = suit[i];
            l++;
        }
    }
}
void shuffle() {
    for (int n = 0; n < 52; n++) {
        int m = rand() % 52;
        temp = deck[n];
        deck[n] = deck[m];
        deck[m] = temp;
    }
}

// =====================================================
// Rozdanie / reset
// =====================================================
void ResetBoard() {
    flopDealt = turnDealt = riverDealt = false;
    flop1 = card{ '?', "" };
    flop2 = card{ '?', "" };
    flop3 = card{ '?', "" };
    turnCard = card{ '?', "" };
    riverCard = card{ '?', "" };
}

void DealNewHand() {
    BuildDeck();
    shuffle();

    top = 0;
    run = 0;
    playerDropped = false;

    ResetBoard();

    player1.resetHand();
    PC.resetHand();

    // PRE-FLOP (na zmianę)
    player1.takeCard(DrawCard());
    PC.takeCard(DrawCard());
    player1.takeCard(DrawCard());
    PC.takeCard(DrawCard());
}

void DealFlop() {
    flop1 = DrawCard();
    flop2 = DrawCard();
    flop3 = DrawCard();
    flopDealt = true;
    run = 1;
}

void DealTurn() {
    turnCard = DrawCard();
    turnDealt = true;
    run = 2;
}

void DealRiver() {
    riverCard = DrawCard();
    riverDealt = true;
    run = 3;
}

// ======
// Game
// ======

string RunName() {
    if (run == 0) return "PRE-FLOP";
    if (run == 1) return "FLOP";
    if (run == 2) return "TURN";
    if (run == 3) return "RIVER";
    return "SHOWDOWN";
}


// =====================================================
// MAIN
// =====================================================

int main() {
    Initialize();

    while (!quitProgram) {
        Render();

        while (!IS_GAME_FINISHED) {
            Update();
            Render();
        }

        // showdown jeśli doszliśmy do końca i nie było quit
        if (!quitProgram) {
            if (!playerDropped && run == 4) {
                Showdown();
            }
        }

        if (quitProgram) break;

        char again;
        cout << "\nPlay again? (y/n): ";
        cin >> again;
        if (again == 'y' || again == 'Y') {
            DealNewHand();
            IS_GAME_FINISHED = false;
        }
        else {
            break;
        }
    }

    Shutdown();
    return 0;
}

// -----------
// FUNKCJE
// -----------

void RenderBoard() {
    cout << "Board:\n";

    cout << "Flop: ";
    if (flopDealt) {
        flop1.printcard(); cout << ", ";
        flop2.printcard(); cout << ", ";
        flop3.printcard();
    }
    else cout << "(not dealt)";
    cout << "\n";

    cout << "Turn: ";
    if (turnDealt) turnCard.printcard();
    else cout << "(not dealt)";
    cout << "\n";

    cout << "River: ";
    if (riverDealt) riverCard.printcard();
    else cout << "(not dealt)";
    cout << "\n";
}

void Render() {
    system("cls"); //czyszczenie planszy przed wyświetleniem etapu
    cout << "\n========================\n";
    cout << "Stage: " << RunName() << "\n\n";

    player1.printHand(false);

    bool hideCpu = (run < 4) && (!playerDropped);
    PC.printHand(hideCpu);

    cout << "\n";
    RenderBoard();

    if (playerDropped) {
        cout << "\nYou deopped. Computer wins.\n";
    }

    cout << "========================\n";
}

void Showdown() {
    if (playerDropped) return;

    // 7 kart: 2 hand + 5 board
    vector<card> p = { player1.hand[0], player1.hand[1], flop1, flop2, flop3, turnCard, riverCard };
    vector<card> c = { PC.hand[0], PC.hand[1], flop1, flop2, flop3, turnCard, riverCard };

    HandValue hvP = BestFrom(p);
    HandValue hvC = BestFrom(c);

    cout << "\n--- SHOWDOWN ---\n";
    cout << "You: " << CategoryName(hvP.cat) << "\n";
    cout << "Computer: " << CategoryName(hvC.cat) << "\n";

    if (hvC < hvP) cout << "Winner: YOU\n";
    else if (hvP < hvC) cout << "Winner: COMPUTER\n";
    else cout << "Result: TIE\n";
}

void Initialize() {
    srand((unsigned)time(nullptr));
    DealNewHand();
    IS_GAME_FINISHED = false;
}

void Update() {

    int choice;
    cout << "\nChoose: 1-next, 2-drop, 3-quit: ";
    cin >> choice;

    if (choice == 3) {
        quitProgram = true;
        IS_GAME_FINISHED = true;
        return;
    }

    if (choice == 2) {
        playerDropped = true;
        run = 4; // koniec rozdania
        IS_GAME_FINISHED = true;
        return;
    }

    if (choice != 1) return;

    if (run == 0) DealFlop();
    else if (run == 1) DealTurn();
    else if (run == 2) DealRiver();
    else {
        run = 4; // showdown
        IS_GAME_FINISHED = true;
    }
}

void Shutdown() {

}