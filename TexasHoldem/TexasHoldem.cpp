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
card deckPlayer_1[2];
card deckPlayer_2[2];

card DrawCard()
{
	return deck[top++]; // zwraca następną kartę i przesuwa top
}



//MAIN
int main()
{
	Initialize(); //Budowa talii, tasowanie
	GetInput();

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
	deckPlayer_1[0] = DrawCard();
	deckPlayer_2[0] = DrawCard();
	deckPlayer_1[1] = DrawCard();
	deckPlayer_2[1] = DrawCard();
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





