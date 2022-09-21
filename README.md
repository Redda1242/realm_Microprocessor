# realm_Microprocessor
microrealms game

To improve this game, I added different effects to make it more engaging.

Firstly, the map was made so that it is randomized everytime the game is
started so the user can enjoy it. This was done by  using the ADC, we read analogue
input from  Port A bit 4. The input analogue data is converted to digital wich can
be seen in the beginning of the game with the outputted numbers and it is used to make map 
random. 

Secondly, 3 LEDS were used that will light up in different events.
This was done by (In main.c) enabling GPIOA and connecting the LEDs in the
Breadboard/Microprocessor to PortA bit 0, Port A bit 5 and Port A bit 6. 
Then making these ports as output using pinMode(Port,bit,I/O)
		pinMode(GPIOA,5,1); 
		pinMode(GPIOA,6,1); 
		pinMode(GPIOA,0,1); 

Then I made a function for each LED in the realm.c to make it turn on and another
function to turn it off. For example for Port A bit 6:
	void YellowOn()
{
	GPIOA->ODR = GPIOA->ODR | (1 << 6); 
}
void YellowOff()
{
	GPIOA->ODR = GPIOA->ODR & ~ (1u << 6); 
}

for turning on , used the OR operator for the port. So it was Port A OR 1(shifted to the 
right 6 times. so if bit 6 was 0 it will become 1 which is output and make LED light up.

I called these LED functions when player would collect health or meet a monster or a dragon.
Also I made sure to use the delay() to make the LED turn on and off slower so that in can be seen.

Also, another improvement is, I made the game start only when the user click the button in the 
bread board to make the game more interactive. Firstly I connected the button in the breadboard 
to PortB bit 4, which is D12 in the board. I made this port as an input. and in the rungame() 
I did :
if (0 == (GPIOB->IDR & (1u << 4))) 
{
	GameStarted = 1;
}
the Game would start by making GameStarted = 1. instead of it depending on clicking 's'.

This means when button is pressed the port B when ANDED with 1 shifted 4 times to the right
will equal 0, which will activate game.


Final improvement was to add musical melodies. In the realm , a data structure was created for 
the notes that will be played, the structure consist of the frequency the buzzer will play , the
duration and the waiting time until next note is played. I used that data structure
and created 9 different musical melodies plus the spring note. Some of these are just an updated 
version of springNote and other are melodies that I looked up the internet and added to them The duration 
and the waiting time. Then used a for loop and the PlayNote function to play these notes with the buzzer.




