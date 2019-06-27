//pinos para controle dos registradores
const int latchPin = 9;
const int dataPin = 8;
const int clockPin = 10;

//pinos dos botoes(ajustar de acordo com o referencial)
const int leftBut = 6;
const int upBut = 4;
const int downBut = 5;
const int rightBut = 3;
const int placeBut = 2;

const long long int interval = 250;
unsigned long long int previousTime = 0;
const int inputDelay = 200;

int previousColor = -1;
int selectedPos = 0;

//Numeros que serao mandados para os registradores
int led[3] = {0, 0, 0};
int color[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

int player = 0;

int bit = 0;

int butRel = 1;
int update = 1;
int lastIn = -1;

int winner = -1;
int cont = 0;

void setup(){
	pinMode(latchPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
	pinMode(clockPin, OUTPUT);

	pinMode(leftBut, INPUT);
	pinMode(upBut, INPUT);
	pinMode(downBut, INPUT);
	pinMode(rightBut, INPUT);
	pinMode(placeBut, INPUT);

	Serial.begin(9600);
}

/**
 *  Olha se o jogador apertou algum botao e atualiza a posicao
 *  do cursor ou marca a posicao como jogada.
**/
void readInput(){
	bit = 2*(selectedPos%3 + 1);

	if(digitalRead(leftBut) == HIGH){
		delay(inputDelay);
		bitClear(led[selectedPos/3], bit);
		bitClear(led[selectedPos/3], bit + 1);

		if(color[selectedPos] != -1) bitSet(led[selectedPos/3], bit + color[selectedPos]);  

		butRel = 0;

		selectedPos--;
		selectedPos = (selectedPos + 9)%9;
		update = 1;

		lastIn = leftBut;
	}else if(digitalRead(upBut) == HIGH){
		delay(inputDelay);
		bitClear(led[selectedPos/3], bit);
		bitClear(led[selectedPos/3], bit + 1);

		if(color[selectedPos] != -1) bitSet(led[selectedPos/3], bit + color[selectedPos]);  

		butRel = 0;
		
		selectedPos-=3;
		selectedPos = (selectedPos + 9)%9;
		update = 1;

		lastIn = upBut;
	}else if(digitalRead(rightBut) == HIGH){
		delay(inputDelay);
		bitClear(led[selectedPos/3], bit);
		bitClear(led[selectedPos/3], bit + 1);

		if(color[selectedPos] != -1) bitSet(led[selectedPos/3], bit + color[selectedPos]);  

		butRel = 0;
		
		selectedPos++;
		selectedPos = (selectedPos + 9)%9;    
		update = 1;

		lastIn = rightBut;
	}else if(digitalRead(downBut) == HIGH){
		delay(inputDelay);

		bitClear(led[selectedPos/3], bit);
		bitClear(led[selectedPos/3], bit + 1);

		if(color[selectedPos] != -1) bitSet(led[selectedPos/3], bit + color[selectedPos]);  

		butRel = 0;
		
		selectedPos+=3;
		selectedPos = (selectedPos + 9)%9;    
		update = 1;

		lastIn = downBut;
	}else if(digitalRead(placeBut) == HIGH){
		delay(inputDelay);

		bitClear(led[selectedPos/3], bit);
		bitClear(led[selectedPos/3], bit + 1);

		if(color[selectedPos] != -1) bitSet(led[selectedPos/3], bit + color[selectedPos]);  

		butRel = 0;
		
		if(color[selectedPos] == -1){
		cont++;

		color[selectedPos] = player;
		
		player = (player + 1)%2;

		bitSet(led[selectedPos/3], bit + color[selectedPos]);

		selectedPos = 0;
		update = 1;
		}

		lastIn = placeBut;
	}

	if(update == 1){
		bit = 2*(selectedPos%3 + 1);

		bitSet(led[selectedPos/3], bit);
		bitSet(led[selectedPos/3], bit + 1);
	}
}	

/**
 *  Olha se a condicao de vitoria foi cumprida para algum dos jogadores.
**/
void checkWin(){
	for(int i=0; i<9; i+=3){
		if(color[i] != -1 && color[i] == color[i+1] && color[i] == color[i+2]){
			winner = color[i];
			return;
		}
	}

	for(int i=0; i<3; i++){
		if(color[i] != -1 && color[i] == color[i+3] && color[i] == color[i+6]){
			winner = color[i];
			return;
		}
	}

	if(color[0] != -1 && color[0] == color[4] && color[0] == color[8]){
		winner = color[0];
		return;
	}
	if(color[2] != -1 && color[2] == color[4] && color[0] == color[6]){
		winner = color[2];
		return;
	}
}

void loop() {
	if(winner == -1){
		checkWin();

		if(cont > 8){//caso o tabuleiro tenha sido preenchido e nenhum jogador ganhou.
			winner = 2;

			led[0] = 252;
			led[1] = 252;
			led[2] = 252;
			
			update = 1;

			cont = 0;
		}else if(winner == 0){
			led[0] = 84;
			led[1] = 84;
			led[2] = 84;
			
			update = 1;

			cont = 0;
		}else if(winner == 1){
			led[0] = 168;
			led[1] = 168;
			led[2] = 168;
			
			update = 1;
			
			cont = 0;
		}else{      
			if(digitalRead(lastIn) == LOW) butRel = 1;

			if(lastIn == -1 || butRel == 1) readInput();
		}
	}

	unsigned long long int currentTime = millis();

	/**
	 *  Olha se deve piscar o cursor ou todos os leds caso o jogo tenha acabado. 
	 **/
	if(currentTime - previousTime >= interval){
		previousTime = currentTime;

		if(winner != -1){
		cont++;
		}else{
			bit = 2*(selectedPos%3 + 1);

			if((led[selectedPos/3] & ((1<<bit)+(1<<(bit+1)))) != 0){
				bitClear(led[selectedPos/3], bit);
				bitClear(led[selectedPos/3], bit + 1);
			}else{
				bitSet(led[selectedPos/3], bit);
				bitSet(led[selectedPos/3], bit + 1);
			}
		}

		update = 1;
	}

	if(update == 1){
		for(int i=2; i>=0; i--){
			digitalWrite(latchPin, LOW);
			shiftOut(dataPin, clockPin, MSBFIRST, (winner != -1 && cont%2 == 0 ? 0:led[i]));
			digitalWrite(latchPin, HIGH);
		}
		update = 0;
	}
}
