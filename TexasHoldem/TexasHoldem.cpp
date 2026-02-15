#include <iostream>
using namespace std;
//constant
const int MAX = 52;

//funkcje
void Initialize();
void GetInput();
void Update();
void Render();
void Shutdown();
void rules();
void shuffle();
void TheDeck();


//zmienne
class card {
public:
	char value;
	string suit;

	void printcard()
	{
		cout << value << " of " << suit;
	}

};
card deck[MAX], temp;

bool IS_GAME_FINISHED = 0;

int top = 0;
//klasa gracza
class Player {
public:
	string name;
	card hand[2];
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

	void printHand() {
		cout << name << " hand: ";
		for (int i = 0; i < cardsInHand; i++) {
			hand[i].printcard();
			if (i < cardsInHand - 1) cout << ", ";
		}
		cout << endl;
	}
};
//card deckPlayer_1[2];
//card deckPlayer_2[2];
Player player1("Player 1");
Player player2("Player 2");
card flop1;
card flop2;
card flop3;
card turn;
card river;

card DrawCard()
{
	return deck[top++]; // zwraca następną kartę i przesuwa top
}



//MAIN
int main()
{
	Initialize(); //Budowa talii, tasowanie
//	GetInput();

	while (!IS_GAME_FINISHED)
	{
		Update();
		Render();
	}

	Shutdown();


	return 0;
}

//Definicje funkcji
void Initialize() 
{
	srand(time(NULL));
	//Budowa talii
	char value[]{ '2','3','4','5','6','7','8','9','T','J','Q','K','A' }; // 13 elementow
	string suit[]{ "Hearts","Diamonds","Clubs","Spades" }; //4 kolory
	int i, j, l = 0;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 13; j++)
		{
			deck[l].value = value[j];
			deck[l].suit = suit[i];
			l++;
		}
	}
	//cout << "The Deck" << endl;
	//TheDeck();
	shuffle();
	cout << endl << "After Shuffle" << endl;
	TheDeck();

	//Dobieranie kart
	top = 0;

	player1.resetHand();
	player2.resetHand();

	player1.takeCard(DrawCard());
	player2.takeCard(DrawCard());
	player1.takeCard(DrawCard());
	player2.takeCard(DrawCard());
	
	//Zwracam co dostali
	player1.printHand();
	player2.printHand();


	flop1 = DrawCard();
	flop2 = DrawCard();
	flop3 = DrawCard();
	turn = DrawCard();
	river = DrawCard();

	//wyświetlam stos

	cout << "Flop: ";
	flop1.printcard(); cout << ", ";
	flop2.printcard(); cout << ", ";
	flop3.printcard(); cout << endl;

	cout << "Turn: ";
	turn.printcard(); cout << endl;

	cout << "River: ";
	river.printcard(); cout << endl;

}
//Zwracanie Talii 
void TheDeck()
{
	int k;
	for (k = 0; k < 52; k++)
	{
		deck[k].printcard();
		cout << endl;
	}

}
void GetInput() 
{
	rules();
}

void rules()
{
	int ch;
	cout << "Would you like to check the basic rules? Press 1 for yes or 2 for no: ";
	cin >> ch;
	if (ch == 1)
	{
		cout << "Placeholder for Basic Rules\n";
	}
	else
	{
		cout << "Lets play" << endl;
	}
}

//Tasowanie
void shuffle()
{
	int n, m;
	for (n = 0; n < 52; n++)
	{
		m = rand() % 52;
		temp = deck[n];
		deck[n] = deck[m];
		deck[m] = temp;
	}
}
void Update()
{

}
void Render() 
{
}
void Shutdown() 
{
}
