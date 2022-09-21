
/*
Copyright (C) 2014  Frank Duignan

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "realm.h"
#include "stm32l031lib.h"

//Prototypes
void SysTick_Handler(void);
void delayms(uint32_t dly);
void randomize(void);
uint16_t ADCRead(void);
void ADCBegin(void);
void GreenOn(void);
void GreenOff(void);
void RedOn(void);
void RedOff(void);
void YellowOn(void);
void YellowOff(void);
void LEDAll(void);

//data structure of musical notes
typedef struct 
{
	uint32_t frequency;
	uint32_t duration;
	uint32_t wait;
} note;

//prototype
void playNote(const note *n);

//defined values and musical notes frequency 
#define A4 440
#define B4 494
#define E4 329
#define C4 262
#define D4 294
#define F4 349
#define Fs4 370
#define Gs4 415
#define A5 880
#define G5 783
#define Fs5 739
#define Cs4 277
#define NOTE_AS3 233
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define ICE_SPELL_COST 10
#define FIRE_SPELL_COST 20
#define LIGHTNING_SPELL_COST 30
#define NOTE_COUNT1 (sizeof(SpringNotes)/sizeof(note))
#define NOTE_COUNT2 (sizeof(Win)/sizeof(note))
#define NOTE_COUNT3 (sizeof(Fail)/sizeof(note))
#define NOTE_COUNT4 (sizeof(Fight)/sizeof(note))

// Find types: h(ealth),s(trength),m(agic),g(old),w(eapon)
static const char FindTypes[]={'h','s','m','g','w'};

//when winning
static const note Win[]={	{NOTE_G4,100,50}, {NOTE_C4,100,50}, {NOTE_DS4,100,50}, {NOTE_F4,100,50}, {NOTE_G4,100,50}, 
														{NOTE_C4,100,50}, {NOTE_DS4,100,50}, {NOTE_F4,100,50},{NOTE_G4,100,50}, {NOTE_C4,100,50}, 
														{NOTE_DS4,100,50}, {NOTE_F4,100,50}, {NOTE_G4,100,50}, {NOTE_C4,100,50},{NOTE_DS4,100,50}, 
														{NOTE_F4,100,50},	{NOTE_G4,100,50}, {NOTE_C4,100,50}, {NOTE_E4,100,50}, {NOTE_F4,100,50}, 
														{NOTE_G4,100,50}, {NOTE_C4,100,50}, {NOTE_E4,100,50}, {NOTE_F4,100,50},{NOTE_G4,100,50}, 
														{NOTE_C4,100,50}, {NOTE_E4,100,50}, {NOTE_F4,100,50}, {NOTE_G4,100,50}, {NOTE_C4,100,50},
														{NOTE_E4,100,50}, {NOTE_F4,100,50}, {NOTE_G4,100,50}, {NOTE_C4,100,50}, {NOTE_DS4,100,50},
														{NOTE_F4,100,50}, {NOTE_G4,100,50}, {NOTE_C4,100,50}, {NOTE_DS4,100,50}, {NOTE_F4,100,50},
														{NOTE_D4,100,50},{NOTE_F4,100,50}, {NOTE_AS3,100,50}, {NOTE_DS4,100,50}, {NOTE_D4,100,50},
														{NOTE_F4,100,50}, {NOTE_AS3,100,50},{NOTE_DS4,100,50}, {NOTE_D4,100,50}, {NOTE_C4,100,50}
};

//fumble
static const note Fail[]={	{NOTE_C5,100,50}, {NOTE_G4,100,50}, {NOTE_E4,100,50} , 	
														{NOTE_A4,100,50}, {NOTE_B4,100,50}, {NOTE_A4,100,50}, 
														{NOTE_GS4,100,50}, {NOTE_AS4,100,50} ,{NOTE_GS4,100,50},
														{NOTE_G4,100,50}, {NOTE_D4,100,50}, {NOTE_E4,100,50}
};
//Spring note music
static const note SpringNotes[]={	{E4,100,50}, {Gs4,100,50}, {Gs4,100,50}, {Gs4,100,50}, {Fs4,100,50},
																	{E4,100,50}, {B4,300,50}, {B4,50,50}, {A4,50,50}, {Gs4,100,50},
																	{E4,100,50}, {E4,100,50},{Fs4,100,50 },{E4,100,50 },{B4,300,50 },
																	{B4,50,50},{A4,50,50},{Gs4,100,50 }
};

//music for begining the game
static const note MusicBegin[]={  {A4,100,100 }, {Gs4,100,100 },{Gs4,100,100 },{Gs4,100,100 },{Fs4,100,100 },
														{A4,100,100 }, {B4,300,100 }, {B4, 50, 100 },{E4,50,100 },{Gs4,100,100 },{A4,100,100 },{A4,100,100 },
														{Fs4,100,100 },{E4,100,100 },{B4,300,100 }, {B4, 50, 100 },{E4,50,100 },{Gs4,100,100 }
													};

//music when meeting dragon
static const note MeetDragon[]={  {Cs4,100,100 }, {Gs4,100,100 },{Gs4,100,100 },{Gs4,100,100 },{Cs4,100,100 },
														{Gs4,100,100 }, {Gs4,300,100 }, {Gs4, 50, 100 },{Cs4,50,100 },{Gs4,100,100 },{Gs4,100,100 },{Gs4,100,100 },
														{Cs4,100,100 },{Gs4,100,100 },{Gs4,300,100 }, {Gs4, 50, 100 },{Cs4,50,100 },{Gs4,100,100 }
													};
//music when losing/dying
static const note Dead[]={  {Fs5,100,100 }, {D4,100,100 },{D4,100,100 },{D4,100,100 },{Fs5,100,100 },
														{D4,100,100 }, {D4,300,100 }, {D4, 50, 100 },{Fs5,50,100 },{D4,100,100 },{D4,100,100 },{D4,100,100 },
														{Fs5,100,100 },{D4,100,100 },{D4,300,100 }, {D4, 50, 100 },{Fs5,50,100 },{D4,100,100 }
													};

//note played when player chose to fight
static const note Fight[]={  {A5,100,50 }};

//note played when user collect something
static const note Collect[]={  {F4,100,50 }};

//note played when play meets a monster
static const note Monster[]={  {Cs4,100,50 }};

//note played when a moving key is pressed
static const note Move[]={  {B4,100,50 }};

static volatile uint32_t SoundDuration = 0;
static volatile uint32_t ms_counter = 0;

// The following arrays define the bad guys and 
// their battle properies - ordering matters!
// Baddie types : O(gre),T(roll),D(ragon),H(ag)
static const char Baddies[]={'O','T','D','H'};

// The following is 4 sets of 4 damage types
static const byte WeaponDamage[]={10,10,5,25,10,10,5,25,10,15,5,15,5,5,2,10};
static const byte FreezeSpellDamage[]={10,20,5,0};
static const byte FireSpellDamage[]={20,10,5,0};
static const byte LightningSpellDamage[]={15,10,25,0};
static const byte BadGuyDamage[]={10,10,15,5};

static int GameStarted = 0;
static tPlayer thePlayer;
static tRealm theRealm;

__attribute__((noreturn)) void runGame(void)  
{
	//randomize map so that a new game starts everytime
	randomize();
	char ch;
	
	//when starting the game play springNotes and turn on the 3 LEDs
	for (unsigned int i=0; i < NOTE_COUNT1; i++)
		{
			
			LEDAll();
			playNote(&SpringNotes[i]); 
			
		}
		
		delayms(500);
		
	eputs("MicroRealms on the STM32L031\r\n");	
	showHelp();		
	
	//Ask user to press button on breadboard to start game
	showGameMessage("Press Button to start a new game\r\n");	
	
	//while loop until button is pressed
	while(GameStarted == 0)
	{
		
		//if button is pressed
		if (0 == (GPIOB->IDR & (1u << 4)))
		{		
			//turn all LEDs
			LEDAll();
			
			//play not MusicBegin
			for (unsigned int i=0; i < NOTE_COUNT1; i++)
			{
				playNote(&MusicBegin[i]); 
			}
			
			//set GameStarted to 1 to start game
			GameStarted = 1;
		}
			
	}
	
	
	
	//show map, player info
	initRealm(&theRealm);	
	initPlayer(&thePlayer,&theRealm);
	showPlayer(&thePlayer);
	showRealm(&theRealm,&thePlayer);
	showGameMessage("Press H for help");
	
	while (1)
	{
		//read users moves (up/down/left/right/help menu)
		ch = getUserInput();
		ch = ch | 32; // enforce lower case
		
		//switch case for user's input
		switch (ch) {
			case 'h' : {
				//show help info
				showHelp();
				break;
			}
			case 'w' : {
				//if w is presses, player take a step up, and the Move note is played
				showGameMessage("UP");
				step('n',&thePlayer,&theRealm);
				for (unsigned int i=0; i < NOTE_COUNT4; i++)
				{
					playNote(&Move[i]); 
		
				}
				break;
			}
			case 's' : {
				//if s is presses, player take a step down ,and the Move note is played
				showGameMessage("DOWN");
				for (unsigned int i=0; i < NOTE_COUNT4; i++)
				{
					playNote(&Move[i]); 
		
				}
				step('s',&thePlayer,&theRealm);
				break;

			}
			case 'd' : {
				//if d is presses, player take a step to the right ,and the Move note is played
				for (unsigned int i=0; i < NOTE_COUNT4; i++)
				{
					playNote(&Move[i]); 
		
				}
				showGameMessage("RIGHT");
				step('e',&thePlayer,&theRealm);
				break;
			}
			case 'a' : {
				//if a is presses, player take a step to the left ,and the Move note is played
				for (unsigned int i=0; i < NOTE_COUNT4; i++)
				{
					playNote(&Move[i]); 
		
				}
				showGameMessage("LEFT");
				step('w',&thePlayer,&theRealm);
				break;
			}
			case '#' : {		
					//show map if # is pressed
					showRealm(&theRealm,&thePlayer);

				break;
			}
			case 'p' : {	
				//show player info if p is pressed
				showPlayer(&thePlayer);
				break;
			}
		} // end switch
	} // end while
}
void step(char Direction,tPlayer *Player,tRealm *Realm)
{
	uint8_t new_x, new_y;
	new_x = Player->x;
	new_y = Player->y;
	byte AreaContents;
	switch (Direction) {
		//update realm according to user moves/direction
		case 'n' :
		{
			if (new_y > 0)
				new_y--;
			break;
		}
		case 's' :
		{
			if (new_y < MAP_HEIGHT-1)
				new_y++;
			break;
		}
		case 'e' :
		{
			if (new_x <  MAP_WIDTH-1)
				new_x++;
			break;
		}
		case 'w' :
		{
			if (new_x > 0)
				new_x--;
			break;
		}		
	}
	AreaContents = Realm->map[new_y][new_x];
	
	//if user hit a rick (astrix)
	if ( AreaContents == '*')
	{
		//turn all LED on
		RedOn();
		YellowOn();
		GreenOn();
		delay(1000000);
		
		//inform user
		showGameMessage("A rock blocks your path.");
		
		//trun LED off
		RedOff();
		YellowOff();
		GreenOff();
		
		return;
	}
	Player->x = new_x;
	Player->y = new_y;
	int Consumed = 0;
	switch (AreaContents)
	{
		// const char Baddies[]={'O','T','B','H'};
		case 'O' :{
			
			//play monster note
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Monster[i]); 
		
			}
			//turn red LED on then off
			RedOn();
			delay(100000);
			RedOff();
			showGameMessage("A smelly green Ogre appears before you");
			Consumed = doChallenge(Player,0);
			break;
		}
		case 'T' :{
			
			//play monster music
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Monster[i]); 
		
			}
			
			//turn all LEDS on 
			for (int i = 0; i < 3; i++)
			{
				LEDAll();
			}			
			showGameMessage("An evil troll challenges you");
			Consumed = doChallenge(Player,1);
			break;
		}
		case 'D' :{
			
			//turn yellow LED on then off
			YellowOn();
			delay(1000000);
			YellowOff();
			
			showGameMessage("A smouldering Dragon blocks your way !");
			
			//play dragon music
			for (unsigned int i=0; i < NOTE_COUNT1; i++)
			{
				playNote(&MeetDragon[i]); 
			}
			Consumed = doChallenge(Player,2);
			break;
		}
		case 'H' :{
			
			//play dragon music
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Monster[i]); 
		
			}
			
			//trun Red LED on then off
			RedOn();
			showGameMessage("A withered hag cackles at you wickedly");
			delay(1000000);
			RedOff();
			Consumed = doChallenge(Player,3);
			break;
		}
		case 'h' :{
			
			//play collecting note
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Collect[i]); 
		
			}
			
			//turn green LED On then off
			GreenOn();
			showGameMessage("You find an elixer of health");
			delay(1000000);
			GreenOff();
			
			//increase health
			setHealth(Player,Player->health+10);
			Consumed = 1;			
			
			break;
			
		}
		case 's' :{
			
			//play collecting note
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Collect[i]); 
		
			}
			
			//turn green LED on then off
			GreenOn();
			showGameMessage("You find a potion of strength");
			Consumed = 1;
			setStrength(Player,Player->strength+1);
			delay(1000000);
			GreenOff();
			break;
		}
		case 'g' :{
			//when gold is collected play collecting note
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Collect[i]); 
		
			}
			
			//turn green LED on then off
			GreenOn();
			delay(1000000);
			GreenOff();
			showGameMessage("You find a shiny golden nugget");
			
			//increase wealth
			Player->wealth++;			
			Consumed = 1;
			break;
		}
		case 'm' :{
			
			//play collecting note
			for (unsigned int i=0; i < NOTE_COUNT4; i++)
			{
				playNote(&Collect[i]); 
		
			}
			
			//green LED on then off
			GreenOn();
			delay(1000000);
			GreenOff();
			showGameMessage("You find a magic charm");
			
			//increase magic
			Player->magic++;						
			Consumed = 1;
			break;
		}
		case 'w' :{
			Consumed = addWeapon(Player,(int)random(MAX_WEAPONS-1)+1);
			showPlayer(Player);
			break;			
		}
		case 'X' : {
			//whne finding a door play collecting note
			for (unsigned int i=0; i < NOTE_COUNT2; i++)
			{
				playNote(&Win[i]); 	
			}
			
			//turn LED on then off
			GreenOn();
			delay(1000000);
			GreenOff();
			// Player landed on the exit
			eputs("A door! You exit into a new realm");
			setHealth(Player,100); // maximize health
			initRealm(&theRealm);
			showRealm(&theRealm,Player);
		}
	}
	if (Consumed)
		Realm->map[new_y][new_x] = '.'; // remove any item that was found
}

//function for fighting monsters
int doChallenge(tPlayer *Player,int BadGuyIndex)
{
	char ch;
	char Damage;
	const byte *dmg;
	int BadGuyHealth = 100;
	eputs("Press F to fight");
	ch = getUserInput() | 32; // get user input and force lower case
	
	//if user chooses to fight
	if (ch == 'f')
	{
		//play fighting note
		for (unsigned int i=0; i < NOTE_COUNT4; i++)
		{
			playNote(&Fight[i]); 

		}
		eputs("\r\nChoose action");
		while ( (Player->health > 0) && (BadGuyHealth > 0) )
		{
			eputs("\r\n");
			// Player takes turn first
			if (Player->magic > ICE_SPELL_COST)
				eputs("(I)CE spell");
			
			if (Player->magic > FIRE_SPELL_COST)
				eputs("(F)ire spell");

			if (Player->magic > LIGHTNING_SPELL_COST)
				eputs("(L)ightning spell");
				
			if (Player->Weapon1)
			{
				eputs("(1)Use ");
				eputs(getWeaponName(Player->Weapon1));
			}	
			if (Player->Weapon2)
			{
				eputs("(2)Use ");
				eputs(getWeaponName(Player->Weapon2));
			}
			eputs("(P)unch");
			ch = getUserInput();
			switch (ch)
			{
				case 'i':
				case 'I':
				{
					eputs("FREEZE!");
					//turn all LED on 
					LEDAll();
					Player->magic -= ICE_SPELL_COST;
					BadGuyHealth -= FreezeSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'f':
				case 'F':
				{
					//turn all LED on 
					LEDAll();
					eputs("BURN!");
					Player->magic -= FIRE_SPELL_COST;
					BadGuyHealth -= FireSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case 'l':
				case 'L':
				{
					//turn all LED on 
					LEDAll();
					eputs("ZAP!");
					Player->magic -= LIGHTNING_SPELL_COST;
					BadGuyHealth -= LightningSpellDamage[BadGuyIndex]+random(10);
					zap();
					break;
				}
				case '1':
				{
					//turn all LED on 
					LEDAll();
					dmg = WeaponDamage+(Player->Weapon1<<2)+BadGuyIndex;
					eputs("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case '2':
				{
					//turn all LED on 
					LEDAll();
					dmg = WeaponDamage+(Player->Weapon2<<2)+BadGuyIndex;
					eputs("Take that!");
					BadGuyHealth -= *dmg + random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				case 'p':
				case 'P':
				{
					//turn all LED on 
					LEDAll();
					eputs("Thump!");
					BadGuyHealth -= 1+random(Player->strength);
					setStrength(Player,Player->strength-1);
					break;
				}
				default: {
					//turn Red LED on and off
					RedOn();
					delay(10000);
					RedOff();
					eputs("You fumble. Uh oh");
					for (unsigned int i=0; i < NOTE_COUNT3; i++)
					{
						playNote(&Fail[i]); 
					}
				}
			}
			// Bad guy then gets a go 
			
			if (BadGuyHealth < 0)
				BadGuyHealth = 0;
			Damage = (uint8_t)(BadGuyDamage[BadGuyIndex]+(int)random(5));
			setHealth(Player,Player->health - Damage);
			eputs("Health: you "); printDecimal(Player->health);
			eputs(", them " );printDecimal((uint32_t)BadGuyHealth);
			eputs("\r\n");
		}
		if (Player->health == 0)
		{ // You died
			eputs("You are dead. Press Reset to restart");
			for (unsigned int i=0; i < NOTE_COUNT1; i++)
		{
			
			LEDAll();
			playNote(&Dead[i]); 
			
		}
			while(1);
		}
		else
		{ // You won!
			Player->wealth = (uint8_t)(50 + random(50));			
			showGameMessage("You win! Their gold is yours");	
			for (unsigned int i=0; i < NOTE_COUNT2; i++)
		{
			

			playNote(&Win[i]); 
			
		}
			//green On and off
			GreenOn();
			delay(10000);
			GreenOff();
			return 1;
		}
		
	}
	else
	{
		showGameMessage("Our 'hero' chickens out");
		return 0;
	}
}
int addWeapon(tPlayer *Player, int Weapon)
{
	char c;
	eputs("You stumble upon ");
	switch (Weapon)
	{
		case 1:
		{	
			eputs("a mighty axe");
			break;
		}
		case 2:
		{	
			eputs("a sword with mystical runes");
			break;
		}
		case 3:
		{	
			eputs("a bloody flail");
			break;
		}		
		default:
			printDecimal((uint32_t)Weapon);
	}
	if ( (Player->Weapon1) && (Player->Weapon2) )
	{
		// The player has two weapons already.
		showPlayer(Player);
		eputs("You already have two weapons\r\n");		
		eputs("(1) drop Weapon1, (2) for Weapon2, (0) skip");
		c = getUserInput();
		eputchar(c);
		switch(c)
		{
			case '0':{
				return 0; // don't pick up
			}
			case '1':{
				Player->Weapon1 = (uint8_t)Weapon;
				break;
			}
			case '2':{
				Player->Weapon2 = (uint8_t)Weapon;
				break;
			}
		}
	}
	else
	{
		if (!Player->Weapon1)
		{
			Player->Weapon1 = (uint8_t)Weapon;	
		}
		else if (!Player->Weapon2)
		{
			Player->Weapon2 = (uint8_t)Weapon;
		}
	}	
	return 1;
}
const char *getWeaponName(int index)
{
	switch (index)
	{
		case 0:return "Empty"; 
		case 1:return "Axe";
		case 2:return "Sword"; 
		case 3:return "Flail"; 
	}
	return "Unknown";
}

//update health so that it is not more than 100 or less than 0
void setHealth(tPlayer *Player,int health)
{
	if (health > 100)
		health = 100;
	if (health < 0)
		health = 0;
	Player->health = (uint8_t)health;
}	

//update strength so that it is not more than 100 
void setStrength(tPlayer *Player, byte strength)
{
	if (strength > 100)
		strength = 100;
	Player->strength = strength;
}
void initPlayer(tPlayer *Player,tRealm *Realm)
{
	// get the player name
	int index=0;
	byte x,y;
	char ch=0;
	// Initialize the player's attributes
	eputs("Enter the player's name: ");
	while ( (index < MAX_NAME_LEN) && (ch != '\n') && (ch != '\r'))
	{
		ch = getUserInput();
		if ( ch > '0' ) // strip conrol characters
		{
			
			Player->name[index++]=ch;
			eputchar(ch);
		}
	}
	Player->name[index]=0; // terminate the name
	setHealth(Player,100);
	Player->strength=(uint8_t)(50+random(50));
	Player->magic=(uint8_t)(50+random(50));	
	Player->wealth=(uint8_t)(10+random(10));
	Player->Weapon1 = 0;
	Player->Weapon2 = 0;
	// Initialize the player's location
	// Make sure the player does not land
	// on an occupied space to begin with
	do {
		x=(uint8_t)random(MAP_WIDTH);
		y=(uint8_t)random(MAP_HEIGHT);
		
	} while(Realm->map[y][x] != '.');
	Player->x=x;
	Player->y=y;
}
void showPlayer(tPlayer *Player)
{
	eputs("\r\nName: ");
	eputs(Player->name);
	eputs("health: ");
	printDecimal(Player->health);
	eputs("\r\nstrength: ");
	printDecimal(Player->strength);
	eputs("\r\nmagic: ");
	printDecimal(Player->magic);
	eputs("\r\nwealth: ");
	printDecimal(Player->wealth);	
	eputs("\r\nLocation : ");
	printDecimal(Player->x);
	eputs(" , ");
	printDecimal(Player->y);	
	eputs("\r\nWeapon1 : ");
	eputs(getWeaponName(Player->Weapon1));
	eputs(" Weapon2 : ");
	eputs(getWeaponName(Player->Weapon2));
}
void initRealm(tRealm *Realm)
{
	unsigned int x,y;
	unsigned int Rnd;
	// clear the map to begin with
	for (y=0;y < MAP_HEIGHT; y++)
	{
		for (x=0; x < MAP_WIDTH; x++)
		{
			Rnd = random(100);
			
			if (Rnd >= 98) // put in some baddies
				Realm->map[y][x]=	Baddies[random(sizeof(Baddies))];
			else if (Rnd >= 95) // put in some good stuff
				Realm->map[y][x]=	FindTypes[random(sizeof(FindTypes))];
			else if (Rnd >= 90) // put in some rocks
				Realm->map[y][x]='*'; 
			else // put in empty space
				Realm->map[y][x] = '.';	
		}
	}
	
	// finally put the exit to the next level in
	x = random(MAP_WIDTH);
	y = random(MAP_HEIGHT);
	Realm->map[y][x]='X';
}

//print map
void showRealm(tRealm *Realm,tPlayer *Player)
{
	int x,y;
	eputs("\r\nThe Realm:\r\n");	
	for (y=0;y<MAP_HEIGHT;y++)
	{
		for (x=0;x<MAP_WIDTH;x++)
		{
			
			if ( (x==Player->x) && (y==Player->y))
				eputchar('@');
			else
				eputchar(Realm->map[y][x]);
		}
		eputs("\r\n");
	}
	eputs("\r\nLegend\r\n");
	eputs("(T)roll, (O)gre, (D)ragon, (H)ag, e(X)it\r\n");
	eputs("(w)eapon, (g)old), (m)agic, (s)trength\r\n");
	eputs("@=You\r\n");
}

//help menu
void showHelp()
{

	eputs("Help\r\n");
	eputs("W,S,D,A : go UP, DOWN, RIGHT, LEFT\r\n");
	eputs("# : show map \r\n");
	eputs("(H)elp\r\n");
	eputs("(P)layer details\r\n");
	
}

void showGameMessage(char *Msg)
{
	eputs(Msg);
	eputs("\r\nReady\r\n");	
}

//read user input
char getUserInput()
{
	char ch = 0;
	
	while (ch == 0)
		ch = egetchar();
	return ch;
}


unsigned random(unsigned range)
{
	// Implementing my own version of modulus
	// as it is a lot smaller than the library version
	// To prevent very long subtract loops, the
	// size of the value returned from prbs has been
	// restricted to 8 bits.
	unsigned Rvalue = (prbs()&0xff);
	while (Rvalue >= range)
		Rvalue -= range; 
	return Rvalue;
}
void zap()
{

}
void ADCBegin()
{
RCC->APB2ENR |= (1u << 9); // Turn on ADC
RCC->IOPENR |= 1; // enable GPIOA
pinMode(GPIOA,4,3); // Make GPIOA_4 an analogue input
ADC1->CR = 0; // disable ADC before making changes
ADC1->CR |= (1u << 28); // turn on the voltage regulator
ADC1->CR |= (1u << 31); // start calibration
while ( (ADC1->CR & (1u << 31)) != 0); // Wait for calibration to complete.
ADC1->CHSELR = (1 << 8); // select channel4  
ADC1->CR |= 1; // enable the ADC
}
uint16_t ADCRead(void)
{
ADC1->CR |= (1 << 2); // start a conversion
while ( (ADC1->CR & (1 << 2)) != 0); // Wait for conversion to complete.
return (uint16_t)ADC1->DR;
}
static unsigned long shift_register=0;

//randomize realm
void randomize()
{
	// To generate a "true" random seed for the prbs generator
	// Repeatedly read a floating analogue input (32 times)
	// and the least significant bit of that read to set each
	// bit of the prbs shift registers.
	// Must make sure that all of its buts are not zero when
	// done.
	RCC->IOPENR |= (1 << 1); // enable port B
	pinMode(GPIOB,0,3); // use PB0 as the source of noise.  This ADC channel 8
	ADCBegin();
	shift_register = 0;
	
	while(shift_register == 0)
	{
		for (int i=0; i < 32; i++)
		{
			shift_register = shift_register << 1;
			shift_register |= (ADCRead() & 1);
			delay(10000);
			printDecimal(shift_register);
			eputs("\r\n");
		}
	}
}
uint32_t prbs()
{

// This is an unverified 31 bit PRBS generator
// It should be maximum length but this has not been verified

unsigned long new_bit=0;

    new_bit = (shift_register >> 27) ^ (shift_register >> 30);
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);

return shift_register & 0x7fffffff; // return 31 LSB's
}

//play melodies function
void playNote(const note *n)
{
	uint32_t Period = 1000000 / n->frequency;
	TIM2->CR1 |= 1; // enable counter 
	TIM2->ARR = Period; // Set initial reload value of 1000 microseconds
	TIM2->CCR2 = TIM2->ARR/2; // 50% duty;
	SoundDuration = n->duration;
	while(SoundDuration != 0); // Wait
	TIM2->ARR = 1;
	TIM2->CR1 |= 0; // enable counter 
	delayms(n->wait);

}

void SysTick_Handler(void)
{
	
	if (SoundDuration > 0)
	{
		SoundDuration --;
	}
	ms_counter ++;   // another millisecond has passed

}
void delayms(uint32_t dly)
{
	dly = dly+ms_counter;
	while(ms_counter < dly);
}

// LED Functions

void GreenOn()
{
	GPIOA->ODR = GPIOA->ODR | (1 << 5);
	//Green LED is in port A bit 5. 
	//To turn LED on we use the OR operator with 1 shifted to the right 5 times. This makes bit 5 = 1.
}
void GreenOff()
{
	GPIOA->ODR = GPIOA->ODR & ~ (1u << 5);
	// To turn LED off we user the AND NOT operator with 1 shifted to the right 5 times, 
	//the NOT will turn it into 0. This means if bit 5 was originally 1 it will be ANDED with 0 and become 0.
}

void YellowOn()
{
	GPIOA->ODR = GPIOA->ODR | (1 << 6);
	//Yellow LED is in port A bit 6. To turn LED on we use the OR with 1 shifted to the right 6 times.
	//	This makes bit 1 = 1. So port A OR (1 << 6) will make bit 6 = 1.
}
void YellowOff()
{
	GPIOA->ODR = GPIOA->ODR & ~ (1u << 6); 
	// To turn LED off we user the AND NOT operator with 1 shifted to the right 6 times, 
	//the NOT will turn the 1 into 0. This means if bit 6 was originally 1 it will be ANDED with 0 and become 0.
}
void RedOn()
{
	GPIOA->ODR = GPIOA->ODR | (1 << 0);
	//Red LED is in port A bit 0. To turn LED on we use the OR operator with 1 shifted to the right 0 times. 
	//This makes bit 0 = 1. So port A OR (1 << 0) will make bit 0 = 1.
}
void RedOff()
{
	GPIOA->ODR = GPIOA->ODR & ~ (1u << 0);
	// To turn LED off we user the AND NOT operator with 1 shifted to the right 0 times, the NOT will turn 1 into 0.
	//	This means if bit 0 was originally 1 it will be ANDED with 0 and become 0.
}

//a pattern for LED turning on and off
void LEDAll()
{
	GPIOA -> ODR |= (1 << 6);
	delay(100000);
	GPIOA -> ODR &= ~(1u << 6);
	delay(10000);
	GPIOA -> ODR |= (1 << 0);
	delay(100000);
	GPIOA -> ODR &= ~(1u << 0);
	delay(10000);
	GPIOA -> ODR |= (1 << 5);
	delay(100000);
	GPIOA -> ODR &= ~(1u << 5);
}
