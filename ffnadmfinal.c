#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>

#define MAXCHARLIMIT 40
#define WIDTH 80
#define HEIGHT 20

#define WALL 'W'
#define PATH ' '
#define PLAYER 'P'
#define GOAL 'G'
#define QUESTION 'Q'

char user[MAXCHARLIMIT];
char *savefile;

char maze[HEIGHT][WIDTH];
int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};

typedef struct {
    int x, y;
} Position;

typedef struct {
    Position player;
    Position goal;
    int timeElapsed;
    bool questionsAnswered[10];
    int totalQuestions;
    int currentFloor;
    bool floorCompleted[5];
}
GameState;

GameState gameState;

int verify(char A[], char B[]){
	if(strlen(A) != strlen(B)) return 0;
	for(int i = 0; i < strlen(A); i++){
		if(A[i] != B[i]) return 0;
	}
	return 1;
}

int hasNum(char C){
	char list[] = "0123456789";
	for(int i = 0; i < strlen(list); i++){
		if(C == list[i]){
			return 1;
		}
	}
	return 0;
}

int hasUpper(char C){
	char list[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for(int i = 0; i < strlen(list); i++){
		if(C == list[i]){
			return 1;
		}
	}
	return 0;
}

int hasSpecial(char C){
	char list[] = "!@#$%%ˆ&*()-+<>,.";
	for(int i = 0; i < strlen(list); i++){
		if(C == list[i]){
			return 1;
		}
	}
	return 0;
}

int strength(char T[]){
	int strength = 0;
	if(8 > strlen(T) || strlen(T) > 33){
		return 3;
	}
	for(int i = 0; i < strlen(T); i++){
		if(hasNum(T[i])){
			strength++;
			break;
		}
	}
	for(int i = 0; i < strlen(T); i++){
		if(hasUpper(T[i])){
			strength++;
			break;
		}
	}
	for(int i = 0; i < strlen(T); i++){
		if(hasSpecial(T[i])){
			strength++;
			break;
		}
	}
	switch(strength){
		case 2:
			return 1;
		case 3:
			return 2;
		default:
			return 0;
	}
}

int LoginOption(char options[][MAXCHARLIMIT], int userAmount) {
    int selected = 0;
    while (1) {
        system("cls");
        if (selected == -1) selected = userAmount - 1;
        if (selected == userAmount) selected = 0;
        printf("Select your user:\n");
        for (int i = 0; i < userAmount; i++) {
            if (selected == i) {
                printf("\033[30m\033[42m");
            }
            printf("[%s]\033[0m", options[i]);
            printf("\n");
        }
        switch (getche()) {
            case 'w':
                selected--;
                break;
            case 's':
                selected++;
                break;
            case ' ':
                return selected;
        }
    }
}

void login() {
    char temp[MAXCHARLIMIT];
    char password[MAXCHARLIMIT];
    char username[MAXCHARLIMIT];
    int userAmount = 0, i = 0, selected = 0, success = 0;
    FILE *n = fopen("UserList.txt", "r");
    if (!n) {
        printf("No user list found. Exiting...\n");
        exit(1);
    }
    while (fscanf(n, "%[^\n]\n", username) != EOF) {
        userAmount++;
    }
    fclose(n);

    char selection[userAmount][MAXCHARLIMIT];
    FILE *f = fopen("UserList.txt", "r");
    while (fscanf(f, "%[^#]#%s\n", username, temp) != EOF) {
        strcpy(selection[i], username);
        i++;
    }
    fclose(f);

    selected = LoginOption(selection, userAmount);

    for (int i = 0; i < 3; i++) {
        system("cls");
        printf("You have %d %s or else you'll be asked to login again\n", 3 - i, (i == 2) ? "try" : "tries");
        printf("Username: %s\n", selection[selected]);
        printf("Password: ");
        scanf("%s", password);
        FILE *x = fopen("UserList.txt", "r");
        while (fscanf(x, "%[^#]#%s\n", username, temp) != EOF) {
            if (verify(username, selection[selected])) {
                if (verify(temp, password)) {
                    success = 1;
                    break;
                }
            }
        }
        fclose(x);
        if (success) break;
    }
    if (!success){
    	login();
		return;
	};

    strcpy(user, selection[selected]);
    savefile = (char*)malloc(strlen(user) + 9);
    sprintf(savefile, "%s_save.txt", user);
}

void newusr(){
	char confirmation;
	char username[MAXCHARLIMIT];
	char password[MAXCHARLIMIT];
	system("cls");
	FILE* f = fopen("UserList.txt","a");
	printf("Enter your username: \n");
	scanf("%s", username); getchar();
	while(confirmation != 'y' && confirmation != 'Y'){
		system("cls");
		printf("Enter your password: \n");
		scanf("%s", password); getchar();
		switch(strength(password)){
			case 0:
				printf("Password is weak, Continue? (Y/N)");
				confirmation = getche();
				break;
			case 1:
				printf("Password is medium, Continue? (Y/N)");
				confirmation = getche();
				break;
			case 2:
				printf("Password is strong, Continue? (Y/N)");
				confirmation = getche();
				break;
			case 3:
				printf("Invalid password, try again");
				Sleep(500);
				break;
		}
	}
	fprintf(f,"%s#%s\n", username, password);
	fclose(f);
	login();
}

void loginpage(){
	int select = 0;
	while(1){
		system("cls");
		printf("Please select your user\n");
		if(select == 0) printf("\033[30m\033[42m");
		printf("[LOG IN]");
		printf("\033[0m\n");
		if(select == 1) printf("\033[30m\033[42m");
		printf("[NEWUSR]");
		printf("\033[0m");
		switch(getche()){
			case 'w':
				select = 0;
				break;
			case 's':
				select = 1;
				break;
			case ' ':
				if(select) newusr();
				else login();
				return;
		}
	}
}

void initializeMaze() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = WALL;
        }
    }
}

void generateMaze(int x, int y) {
    int dirs[] = {0, 1, 2, 3};
    maze[y][x] = PATH;
    
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp = dirs[i];
        dirs[i] = dirs[r];
        dirs[r] = temp;
    }

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[dirs[i]] * 2;
        int ny = y + dy[dirs[i]] * 2;
        if (nx > 0 && ny > 0 && nx < WIDTH - 1 && ny < HEIGHT - 1 && maze[ny][nx] == WALL) {
            maze[y + dy[dirs[i]]][x + dx[dirs[i]]] = PATH;
            generateMaze(nx, ny);
        }
    }
}

void updatePlayerPosition(Position oldPos, Position newPos) {
    void setCursorPosition(int x, int y);
    
    setCursorPosition(oldPos.x, oldPos.y);
    putchar(PATH);

    setCursorPosition(newPos.x, newPos.y);
    putchar(PLAYER);

    setCursorPosition(0, HEIGHT + 1);
    printf("Time: %d seconds   ", gameState.timeElapsed);
    printf("\nUse WASD to move. Press Q to quit.\n");
}

void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

Position findOpenPath() {
    Position pos;
    do {
        pos.x = rand() % (WIDTH - 2) + 1;
        pos.y = rand() % (HEIGHT - 2) + 1;
    } while (maze[pos.y][pos.x] != PATH);
    return pos;
}

void saveGame() {
    FILE *file = fopen(savefile, "w");
    if (!file) {
        perror("Error saving game");
        return;
    }
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            fputc(maze[y][x], file);
        }
        fputc('\n', file);
    }
    
    fprintf(file, "%d %d\n", gameState.player.x, gameState.player.y);
    fprintf(file, "%d %d\n", gameState.goal.x, gameState.goal.y);
    fprintf(file, "%d\n", gameState.timeElapsed);
    fprintf(file, "%d\n", gameState.totalQuestions);
    fprintf(file, "%d\n", gameState.currentFloor);
    fprintf(file, "%d\n", (int)time(NULL));

    for (int i = 0; i < gameState.totalQuestions; i++) {
        fprintf(file, "%d ", gameState.questionsAnswered[i]);
    }
    fprintf(file, "\n");
    
    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d ", gameState.floorCompleted[i]);
    }
    fprintf(file, "\n");
    
    fclose(file);
    printf("Game saved successfully!\n");
}

bool loadGame() {
    FILE *file = fopen(savefile, "r");
    if (!file) {
        printf("No save file found for %s. Starting a new game.\n", user);
        return false;
    }

    char line[WIDTH + 2];
    for (int y = 0; y < HEIGHT; y++) {
        if (fgets(line, WIDTH + 2, file) == NULL) {
            fclose(file);
            return false;
        }
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = line[x];
        }
    }
    
    time_t savedTime;
    fscanf(file, "%d %d\n", &gameState.player.x, &gameState.player.y);
    fscanf(file, "%d %d\n", &gameState.goal.x, &gameState.goal.y);
    fscanf(file, "%d\n", &gameState.timeElapsed);
    fscanf(file, "%d\n", &gameState.totalQuestions);
    fscanf(file, "%d\n", &gameState.currentFloor);
    fscanf(file, "%d\n", (int*)&savedTime);
    
    time_t currentTime = time(NULL);
    gameState.timeElapsed += (int)(currentTime - savedTime);
    
    for (int i = 0; i < gameState.totalQuestions; i++) {
        fscanf(file, "%d", &gameState.questionsAnswered[i]);
    }
    
    for (int i = 0; i < 5; i++) {
        fscanf(file, "%d", &gameState.floorCompleted[i]);
    }
    
    fclose(file);
    printf("Game loaded successfully!\n");
    
    // Bubble sort
    for (int i = 0; i < gameState.totalQuestions - 1; i++) {
        for (int j = 0; j < gameState.totalQuestions - i - 1; j++) {
            if (strcmp(selection[j], selection[j + 1]) > 0) {
                char temp[MAXCHARLIMIT];
                strcpy(temp, selection[j]);
                strcpy(selection[j], selection[j + 1]);
                strcpy(selection[j + 1], temp);
            }
        }
    }
    
    return true;
}

void handleQuestions(Position questionPositions[], int count) {
    for (int i = 0; i < gameState.totalQuestions; i++) {
        if (!gameState.questionsAnswered[i] && 
            maze[gameState.player.y][gameState.player.x] == QUESTION) {
            
            system("cls");
            printf("Time: %d seconds\n", gameState.timeElapsed);
            printf("Floor: %d\n\n", gameState.currentFloor);
            printf("Level %d Question\n", 5 - i);
            printf("===================\n\n");
            
            // Tampilkan pertanyaan
            printf("=== Question ===\n");
            switch (5 - i) {
                case 5:
                    Level5Questions();
                    //Only in this choice if that player chose number 1, that player can automatically find BAD Ending.
                    printf("\n\nYour choice:\n");
                    printf("1. Seal the deal\n");
                    printf("2. Squaring up with Diddy\n");
                    printf("3. Trying to shake Diddy off\n");
                    printf("4. Tearing up the contract\n");
                    break;
                case 4:
                    Level4Questions();
                    printf("\n\nWhatcu gonna do?\n");
                    printf("1. Getting juiced up by Diddy\n");
                    printf("2. Pushing back\n");
                    printf("3. Working out a deal\n");
                    printf("4. Getting the hell out\n");
                    break;
                case 3:
                    Level3Questions();
                    printf("\n\nHow you gonna react?\n");
                    printf("1. Snagging some baby oil\n");
                    printf("2. Stepping up to Diddy\n");
                    printf("3. Figuring an exit\n");
                    printf("4. Faking a faint\n");
                    break;
                case 2:
                    Level2Questions();
                    printf("\n\nWhat's your move?\n");
                    printf("1. Chasing the noise\n");
                    printf("2. Stayin' in the cut\n");
                    printf("3. Screamin' for a save'\n");
                    printf("4. Grabbing some steel\n");
                    break;
                case 1:
                    Level1Questions();
                    printf("\n\nWhat you tryna to pull?\n");
                    printf("1. Keepin' it cool\n");
                    printf("2. Rollin' up on him\n");
                    printf("3. Dippin' outta here\n");
                    printf("4. Askin' for the lowdown\n");
                    break;
            }
            
            printf("\nYour pick (1-4): ");
            int answer;
            scanf("%d", &answer);
            
            // Tampilkan hasil jawaban
            printf("\n=== The final word ===\n");
            switch (5 - i) {
                case 5:
                    handleLevel5Answer(answer);
                    break;
                case 4:
                    handleLevel4Answer(answer);
                    break;
                case 3:
                    handleLevel3Answer(answer);
                    break;
                case 2:
                    handleLevel2Answer(answer);
                    break;
                case 1:
                    handleLevel1Answer(answer);
                    break;
            }
            
            printf("\nPress any key to continue...");
            getch();
            
            gameState.questionsAnswered[i] = true;
            maze[gameState.player.y][gameState.player.x] = PATH;

            // Refresh layar maze
            system("cls");
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    if (gameState.player.x == x && gameState.player.y == y)
                        putchar(PLAYER);
                    else if (maze[y][x] == WALL)
                        printf("\033[106m \033[0m");
                    else if (x == gameState.goal.x && y == gameState.goal.y)
                        putchar(GOAL);
                    else
                        putchar(maze[y][x]);
                }
                putchar('\n');
            }
            printf("\nTime: %d seconds", gameState.timeElapsed);
            printf("\nFloor: %d", gameState.currentFloor);
            printf("\nUse WASD to move. Press Q to quit.\n");
            break;
        }
    }
}

void Level5Questions() {
    int questionNumber = rand() % 1; // Hanya 1 pertanyaan
    switch(questionNumber) {
        case 0:
            animatedText("He's pushin' some sus paperwork.\n");
            animatedText("'ink this and fame's is yours,' he said with a shady-ass smile.\n");
            break;
    }
}

void Level4Questions() {
    int questionNumber = rand() % 2; // 2 pertanyaan
    switch(questionNumber) {
        case 0:
            animatedText("He's got a shot full of sus juice, and it ain't lookin good bruh.\n");
            animatedText("'This'll chill you out, darling,' he breathes into your ears.\n");
            break;
        case 1:
            animatedText("Outta nowhere,this sketchy door shows up right in your face.\n");
            animatedText("'Care to roll the dice darling?' Diddy says,laughing like he's got plans.\n");
            break;
    }
}

void Level3Questions() {
    int questionNumber = rand() % 3; // 3 pertanyaan
    switch(questionNumber) {
        case 0:
            animatedText("The floor's slick with baby oil,it's a freakin' trap.\n");
            animatedText("'Let's seee how quick you really are,'Diddy sneers.'\n");
            break;
        case 1:
            animatedText("Mirrors all around,Diddy's reflections are messin' with your head.\n");
            animatedText("'Think you can spot real me?'\n");
            break;
        case 2:
            animatedText("Diddy's laugh echoes everywhere, it's creepy ass hell.\n");
            animatedText("'You ain't escapin' this, no chance!.'\n");
            break;
    }
}

void Level2Questions() {
    int questionNumber = rand() % 4; // 4 pertanyaan
    switch(questionNumber) {
        case 0:
            animatedText("The lights keep flickering,showin' creapy-ass shadows.\n");
            animatedText("'scared of the dark,huh?'Diddy sneers with a sly grin.'\n");
            break;
        case 1:
            animatedText("You hear footsteps creepin' closer,real slow-like.\n");
            animatedText("'Ready or not,here i come!!'\n");
            break;
        case 2:
            animatedText("Doors around you slam shut, one by one-no way out.\n");
            animatedText("'Make your pick,but don't screw it up,' Diddy warns,smirking.\n");
            break;
        case 3:
            animatedText("An old-ass music box starts palyin' on its own,all creapy and stuff.\n");
            animatedText("'Shall we dance?,'Diddy asks,lookin' way to smug.\n");
            break;
    }
}

void Level1Questions() {
    int questionNumber = rand() % 5; // 5 pertanyaan
    switch(questionNumber) {
        case 0:
            animatedText("Well, well... trapped like a lil' mouse in my maze, huh?.\n");
            animatedText("'This is your last shot, sweetheart. Or should I say... your last breath?,' Diddy murmurs.\n");
            break;
        case 1:
            animatedText("See that clock? Same time, over and over... just like you, stuck in my maze.\n");
            animatedText("'Time doesn't move here, sugar. It just... stops. Like you will soon.',' sneered Diddy.\n");
            break;
        case 2:
            animatedText("That doll? oh, she thought she was clever, tried outsmart me.\n");
            animatedText("'Now she's just part of the decor. You'll fit right in,' laughed Diddy.\n");
            break;
        case 3:
            animatedText("Oh you feel that? That's my mansion saying hello. She loves to squeeze tight.\n");
            animatedText("'Let's see how long you last before she crushes you,' taunted Diddy.\n");
            break;
        case 4:
            animatedText("Take a good look in that mirror, sugar. That's your pathetic little past staring back at you.\n");
            animatedText("'And soon? You'll be nothing more than my favorite little reflection,' hissed Diddy, his voice dripping with malice.\n");
            break;
    }
}

void handleLevel5Answer(int answer) {
    switch(answer) {
        case 1:
            animatedText("You signed the contract...\n");
            animatedText("Diddy chuckled, his eyes glamming. 'Smart move, kid. Now your soul's my little plaything.'\n");
            break;
        case 2:
            animatedText("You decided to fight Diddy...\n");
            animatedText("Diddy smirked, tilting his head. 'Oh, you got guts, huh? Let's see how quick I can crush 'em.'\n");
            break;
        case 3:
            animatedText("You tried to run...\n");
            animatedText("Diddy blocked the door, grinning wide. 'Run all you want, sugar. This place eats cowards alive.'\n");
            break;
        case 4:
            animatedText("You ripped the contract in front of Diddy...\n");
            animatedText("Diddy smile vanished, his voice dropping low. 'Big mistake, sweetheart. Now, it's game over.'\n");
            break;
    }
}

void handleLevel4Answer(int answer) {
    switch(answer) {
        case 1:
            animatedText("You take the shot...\n");
            animatedText("Your vision fades as Diddy whispers, 'Nighty night, kid. Sweet nightmares.'\n");
            break;
        case 2:
            animatedText("You refuse and fight back...\n");
            animatedText("Diddy sneers, 'Stubborn little thing, aren't you? Let's see how long you last.'\n");
            break;
        case 3:
            animatedText("You try to negotiate...\n");
            animatedText("Diddy laughs coldly, 'Negotiate? In my mansion? That's adorable, kid.'\n");
            break;
        case 4:
            animatedText("You try to escape...\n");
            animatedText("Diddy tilts his head, watching you. 'Run, run, little mouse. I love a good chase.'\n");
            break;
    }
}

void handleLevel3Answer(int answer) {
    switch(answer) {
        case 1:
            animatedText("You grab one of the baby oil bottles...\n");
            animatedText("Diddy's eyes light up, a twisted smile spreading across his face. 'Oh, touching my collection, are we? You better treat her gently. That's bottle number 438, my favorite.'\n");
            break;
        case 2:
            animatedText("You step up to Diddy, standing your ground...\n");
            animatedText("Diddy's grin widens, his eyes glinting with amusement. 'Oh, look at you! Got guts, kid. Too bad guts don't mean a thing in my house.'\n");
            break;
        case 3:
            animatedText("You scan the room, looking for a way out...\n");
            animatedText("Diddy watches you, his voice dripping with mockery. 'Oh, looking for an escape? This house doesn't let anyone leave, darling.'\n");
            break;
        case 4:
            animatedText("You drop to the floor, pretending to faint...\n");
            animatedText("Diddy tilts his head, laughing. 'What is this, amateur hour? Get up, sweetheart. You're not fooling anyone.'\n");
            break;
    }
}

void handleLevel2Answer(int answer) {
    switch(answer) {
        case 1:
            animatedText("You run toward the noise, heart pounding...\n");
            animatedText("Diddy's laughter echoes behind you. 'Oh, you like chasing shadows, huh? They'll lead you right back to me.'\n");
            break;
        case 2:
            animatedText("You hide in the shadows, holding your breath...\n");
            animatedText("Diddy's voice cuts through the dark. 'Hiding, are we? I can smell fear, and trust me, you stink of it.'\n");
            break;
        case 3:
            animatedText("You scream, hoping someone will hear you...\n");
            animatedText("Diddy appears from the shadows, grinning wide. 'Scream louder, baby. No one's coming. It's just you and me now.'\n");
            break;
        case 4:
            animatedText("You grab a steel pipe, clutching it tightly...\n");
            animatedText("Diddy claps slowly, mockingly. 'Oh, the hero act. I love it! Come on, show me what you got, tough guy.'\n");
            break;
    }
}

void handleLevel1Answer(int answer) {
    switch(answer) {
        case 1:
            animatedText("You try to stay calm, steadying your breath...\n");
            animatedText("Diddy leans in close, his voice a low hiss. 'Calm? That's cute. Let's see how calm you are when I'm done with you.'\n");
            break;
        case 2:
            animatedText("You swing at Diddy with everything you've got...\n");
            animatedText("Diddy catches your attack with ease, grinning. 'That all you got? Weak. You're just making this more fun for me.'\n");
            break;
        case 3:
            animatedText("You dash for the door, desperate to escape...\n");
            animatedText("Diddy appears in front of you, laughing darkly. 'Going somewhere? This mansion doesn't let its toys leave, sweetheart.'\n");
            break;
        case 4:
            animatedText("You stand your ground, demanding answers...\n");
            animatedText("Diddy chuckles, his eyes glinting. 'Answers? Oh, you'll get them... piece by piece. Let's start with your screams.'\n");
            break;
    }
}

/* Main Game Logic */
void MazeGame() {
    bool loadedGame = loadGame();
    time_t startTime;
    int lastTimeDisplay = -1;
    
    if (!loadedGame) {
        gameState.currentFloor = 5;
        gameState.timeElapsed = 0;
        startTime = time(NULL);
        
        generateNewFloor();
    } else {
        startTime = time(NULL) - gameState.timeElapsed;
    }

    // Tampilkan maze awal
    system("cls");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (gameState.player.x == x && gameState.player.y == y)
                putchar(PLAYER);
            else if (maze[y][x] == WALL)
                printf("\033[106m \033[0m");
            else if (x == gameState.goal.x && y == gameState.goal.y) {
                bool allAnswered = true;
                for (int i = 0; i < gameState.totalQuestions; i++) {
                    if (!gameState.questionsAnswered[i]) {
                        allAnswered = false;
                        break;
                    }
                }
                putchar(allAnswered ? GOAL : PATH);
            }
            else
                putchar(maze[y][x]);
        }
        putchar('\n');
    }
    printf("\nTime: %d seconds", gameState.timeElapsed);
    printf("\nFloor: %d", gameState.currentFloor);
    printf("\nUse WASD to move. Press Q to quit.\n");

    while (1) {
        Position oldPos = gameState.player;
        
        // Cek apakah semua pertanyaan sudah dijawab
        bool allQuestionsAnswered = true;
        for (int i = 0; i < gameState.totalQuestions; i++) {
            if (!gameState.questionsAnswered[i]) {
                allQuestionsAnswered = false;
                break;
            }
        }

        // Tampilkan goal hanya jika semua pertanyaan sudah dijawab
        if (allQuestionsAnswered) {
            maze[gameState.goal.y][gameState.goal.x] = GOAL;
        }

        // Cek jika pemain mencapai goal
        if (gameState.player.x == gameState.goal.x && 
            gameState.player.y == gameState.goal.y && 
            allQuestionsAnswered) {
            transitionToNextFloor();
            if (gameState.currentFloor > 0) {
                saveGame();
                continue; // Lanjut ke floor berikutnya
            } else {
                saveGame();
                break; // Kembali ke menu utama
            }
        }

        if (_kbhit()) {
            char input = _getch();
            if (input == 'q' || input == 'Q') {
                saveGame();
                break;
            }
            
            Position newPos = gameState.player;
            
            switch(input) {
                case 'w': case 'W': newPos.y--; break;
                case 's': case 'S': newPos.y++; break;
                case 'a': case 'A': newPos.x--; break;
                case 'd': case 'D': newPos.x++; break;
                default: continue;
            }

            if (maze[newPos.y][newPos.x] != WALL) {
                gameState.player = newPos;
                
                // Update layar hanya untuk posisi yang berubah
                setCursorPosition(oldPos.x, oldPos.y);
                if (oldPos.x == gameState.goal.x && oldPos.y == gameState.goal.y && allQuestionsAnswered)
                    putchar(GOAL);
                else
                    putchar(maze[oldPos.y][oldPos.x]);
                
                setCursorPosition(newPos.x, newPos.y);
                putchar(PLAYER);
            }
        }

        // Handle questions
        if (maze[gameState.player.y][gameState.player.x] == QUESTION) {
            handleQuestions(NULL, 0); // Parameter disesuaikan dengan implementasi handleQuestions
            
            if (allQuestionsAnswered) {
                maze[gameState.goal.y][gameState.goal.x] = GOAL;
                // Refresh layar
                system("cls");
                for (int y = 0; y < HEIGHT; y++) {
                    for (int x = 0; x < WIDTH; x++) {
                        if (gameState.player.x == x && gameState.player.y == y)
                            putchar(PLAYER);
                        else if (maze[y][x] == WALL)
                            printf("\033[106m \033[0m");
                        else if (x == gameState.goal.x && y == gameState.goal.y)
                            putchar(GOAL);
                        else
                            putchar(maze[y][x]);
                    }
                    putchar('\n');
                }
            }
        }

        // Update waktu
        gameState.timeElapsed = (int)(time(NULL) - startTime);
        if (gameState.timeElapsed != lastTimeDisplay) {
            setCursorPosition(0, HEIGHT + 1);
            printf("Time: %d seconds   ", gameState.timeElapsed);
            printf("\nFloor: %d", gameState.currentFloor);
            printf("\nUse WASD to move. Press Q to quit.\n");
            lastTimeDisplay = gameState.timeElapsed;
        }

        Sleep(50); // Mengurangi penggunaan CPU
    }
}

/* Start game function */
void startPlay() {
    srand(time(0));
    login();
    
    printf("Playing: Press any key");
    getche(); 
	
	system("cls");
	getche();
	printf("\033[40m\033[91m");
	animatedText("You wake up to cold stone beneath your hands. The air smells sharp-baby oil mixed with something metallic.\n");
	animatedText("Dim light flickers above, casting eerie shadows on the cracked walls around you.\n");
	animatedText("Your head throbs as you take in the room: racks of bizarre costumes, shattered glass, and syringes scattered across the floor.\n"); 
	animatedText("The faint sound of dripping water echoes through the space, each drop sending a chill down your spine.\n");
	animatedText("The walls suddenly creak and groan. The floor shifts beneath you, tilting slightly as the room seems to rearrange itself.\n");
	animatedText("A narrow, dark corridor stretches ahead, its edges flickering like it's alive.\n");
	animatedText("A distorted laugh echoes faintly, followed by a single whisper:\n");
	animatedText("Run, sweetheart. Let's see if you're worth the invitation.\n");
	animatedText("The corridor looms before you, hungry and alive, daring you to take the first step.\n");
	printf("\033[0m");
    
	MazeGame();
}

void config() {
    printf("Press any key");
    getche();
}

void animatedText(char text[]) {
	int i,skip = 0;
	for(i = 0; i < strlen(text); i++) {
		printf("%c",text[i]);
		if(kbhit()) {
			getch();
			skip = 1;
		}
		if(skip) continue;
		if(text[i] == '.') Sleep(200);
		Sleep(50);
	}
	return;
}

void generateNewFloor() {
    // Reset dan generate maze baru
    initializeMaze();
    generateMaze(1, 1);
    
    // Set posisi awal player
    gameState.player = (Position){1, 1};
    
    // Tempatkan pertanyaan di maze
    gameState.totalQuestions = 6 - gameState.currentFloor;
    memset(gameState.questionsAnswered, 0, sizeof(gameState.questionsAnswered));
    
    // Tempatkan goal di posisi random yang valid
    gameState.goal = findOpenPath();
    
    // Tempatkan pertanyaan di maze
    for (int i = 0; i < gameState.totalQuestions; i++) {
        Position qPos;
        do {
            qPos = findOpenPath();
        } while ((qPos.x == gameState.goal.x && qPos.y == gameState.goal.y) || 
                 (qPos.x == gameState.player.x && qPos.y == gameState.player.y) ||
                 maze[qPos.y][qPos.x] == QUESTION);
        
        maze[qPos.y][qPos.x] = QUESTION;
    }

    // Goal tidak ditampilkan di awal
    maze[gameState.goal.y][gameState.goal.x] = PATH;
}

void displayTransitionScreen() {
    system("cls");
    printf("\n\n");
    printf("Floor %d Completed!\n", gameState.currentFloor);
    printf("Time Elapsed: %d seconds\n", gameState.timeElapsed);
    
    if (gameState.currentFloor > 1) {
        printf("Preparing Floor %d...\n", gameState.currentFloor - 1);
        printf("Press any key to continue...");
    } else {
        printf("CONGRATULATIONS!\n");
        printf("You've completed all floors!\n");
        printf("Total Time: %d seconds\n", gameState.timeElapsed);
        printf("Press any key to return to main menu...");
    }
    getch();
}

void transitionToNextFloor() {
    // Simpan status penyelesaian lantai saat ini
    gameState.floorCompleted[5 - gameState.currentFloor] = true;
    
    // Tampilkan layar transisi
    displayTransitionScreen();
    
    // Jika masih ada lantai berikutnya
    if (gameState.currentFloor > 1) {
        // Pindah ke lantai berikutnya
        gameState.currentFloor--;
        
        // Generate lantai baru
        generateNewFloor();
        
        // Simpan progress
        saveGame();
        
        // Refresh tampilan
        system("cls");
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (gameState.player.x == x && gameState.player.y == y)
                    putchar(PLAYER);
                else if (maze[y][x] == WALL)
                    printf("\033[106m \033[0m");
                else if (x == gameState.goal.x && y == gameState.goal.y)
                    putchar(GOAL);
                else
                    putchar(maze[y][x]);
            }
            putchar('\n');
        }
        
        // Tampilkan informasi status
        printf("\nTime: %d seconds", gameState.timeElapsed);
        printf("\nFloor: %d", gameState.currentFloor);
        printf("\nUse WASD to move. Press Q to quit.\n");
    } else {
        // Game selesai, simpan progress terakhir
        saveGame();
    }
}

int main() {
    char ascii[10000];
    char asciiart[8][269];
    int loops = 0, opt = 0, arrowAnim = 0, exit = 0;
    int i, j;
    char temp;

    strcpy(asciiart[0], " ________ ___  ___      ___ _______           ________   ___  ________  ___  ___  _________  ________           ________  _________        ________  ___  ________  ________      ___    ___      _____ ______   ________  ________   ________  ___  ________  ________      ");
    strcpy(asciiart[1], "|\\  _____\\\\  \\|\\  \\    /  /|\\  ___ \\         |\\   ___  \\|\\  \\|\\   ____\\|\\  \\|\\  \\|\\___   ___\\\\   ____\\         |\\   __  \\|\\___   ___\\     |\\   ___ \\|\\  \\|\\   ___ \\|\\   ___ \\    |\\  \\  /  /|    |\\   _ \\  _   \\|\\   __  \\|\\   ___  \\|\\  \\|\\   __  \\|\\   ___  \\    ");
    strcpy(asciiart[2], "\\ \\  \\__/\\ \\  \\ \\  \\  /  / | \\   __/|        \\ \\  \\\\ \\  \\ \\  \\ \\  \\___|\\ \\  \\\\\\  \\|___ \\  \\_\\ \\  \\___|_        \\ \\  \\|\\  \\|___ \\  \\_|     \\ \\  \\_|\\ \\ \\  \\ \\  \\_|\\ \\ \\  \\_|\\ \\   \\ \\  \\/  / /    \\ \\  \\\\\\__\\ \\  \\ \\  \\|\\  \\ \\  \\\\ \\  \\ \\  \\___|\\ \\  \\ \\  \\|\\  \\ \\  \\\\ \\  \\   ");
    strcpy(asciiart[3], " \\ \\   __\\\\ \\  \\ \\  \\/  / / \\ \\  \\_|/__       \\ \\  \\\\ \\  \\ \\  \\ \\  \\  __\\ \\   __  \\   \\ \\  \\ \\ \\_____  \\        \\ \\   __  \\   \\ \\  \\       \\ \\  \\ \\\\ \\ \\  \\ \\  \\ \\\\ \\ \\  \\ \\\\ \\   \\ \\    / /      \\ \\  \\\\|__| \\  \\ \\   __  \\ \\  \\\\ \\  \\ \\_____  \\ \\  \\ \\  \\\\\\  \\ \\  \\\\ \\  \\  ");
    strcpy(asciiart[4], "  \\ \\  \\_| \\ \\  \\ \\    / /   \\ \\  \\_|\\ \\       \\ \\  \\\\ \\  \\ \\  \\ \\  \\|\\  \\ \\  \\ \\  \\   \\ \\  \\ \\|____|\\  \\        \\ \\  \\ \\  \\   \\ \\  \\       \\ \\  \\_\\\\ \\ \\  \\ \\  \\_\\\\ \\ \\  \\_\\\\ \\   \\/  /  /        \\ \\  \\    \\ \\  \\ \\  \\ \\  \\ \\  \\\\ \\  \\|____|\\  \\ \\  \\ \\  \\\\\\  \\ \\  \\\\ \\  \\ ");
    strcpy(asciiart[5], "   \\ \\__\\   \\ \\__\\ \\__/ /     \\ \\_______\\       \\ \\__\\\\ \\__\\ \\__\\ \\_______\\ \\__\\ \\__\\   \\ \\__\\  ____\\_\\  \\        \\ \\__\\ \\__\\   \\ \\__\\       \\ \\_______\\ \\__\\ \\_______\\ \\_______\\__/  / /           \\ \\__\\    \\ \\__\\ \\__\\ \\__\\ \\__\\\\ \\__\\____\\_\\  \\ \\__\\ \\_______\\ \\__\\\\ \\__\\");
    strcpy(asciiart[6], "    \\|__|    \\|__|\\|__|/       \\|_______|        \\|__| \\|__|\\|__|\\|_______|\\|__|\\|__|    \\|__| |\\_________\\        \\|__|\\|__|    \\|__|        \\|_______|\\|__|\\|_______|\\|_______|\\___/ /             \\|__|     \\|__|\\|__|\\|__|\\|__| \\|__|\\_________\\|__|\\|_______|\\|__| \\|__|");
    strcpy(asciiart[7], "                                                                                               \\|_________|                                                                     \\|___|/                                                 \\|_________|                         ");

	loginpage();
	system("cls");   
   
    animatedText("DISCLAIMER\n\nThis game contains mature content that may not be suitable for a younger audience\n");
    Sleep(100);
    printf("Please turn away if you're not older than 18\n");
    Sleep(100);
    animatedText("Press any key\n");
    getche();

    while (1) {
        while (!exit) {
            system("cls");
            sprintf(ascii, "%.100s\n%.100s\n%.100s\n%.100s\n%.100s\n%.100s\n%.100s\n%.100s\n\n",
                    asciiart[0], asciiart[1], asciiart[2], asciiart[3], 
                    asciiart[4], asciiart[5], asciiart[6], asciiart[7]);
    
            if (kbhit()) { 
                char c = getch(); 
                if (c == 'w') opt--;
                if (c == 's') opt++;
                if (c == ' ') break;
            }  
    
            if (opt == 4) opt = 0;
            if (opt == -1) opt = 3;
    
            printf("\033[31m%s\033[0m", ascii);
            printf("\nWelcome back %s!\n", user);
            if (opt == 0) 
                printf((arrowAnim) ? "\033[32m--> " : "\033[32m -->");
            printf("  Play\n\033[0m");
            
            if (opt == 1) 
                printf((arrowAnim) ? "\033[32m--> " : "\033[32m -->");
            printf("  Log Out\n\033[0m");
            
            if (opt == 2) 
                printf((arrowAnim) ? "\033[32m--> " : "\033[32m -->");
            printf("  Credits\n\033[0m");
            
            if (opt == 3) 
                printf((arrowAnim) ? "\033[32m--> " : "\033[32m -->");
            printf("  Exit\n\033[0m");
    		
            printf("\n\033[33mTips! Move use WASD and SPACE\033[0m");
    
            exit = exit || (++loops > 1000);
    
            arrowAnim = (arrowAnim) ? 0 : 1;
            for (i = 0; i < 8; i++) {
                temp = asciiart[i][0];
                for (j = 1; j < 269; j++) 
                    asciiart[i][j - 1] = asciiart[i][j];
                asciiart[i][268] = temp;
            } 
            Sleep(250);
        }
    
        switch (opt) {
            case 0:
                startPlay(); break;
            case 1:
                loginpage(); break;
            case 2:
                printf("Five nights at diddy mansion 2024 oleh Samuel, Fatih, Albani, dan Farhan(?)\nPress any key"); 
                getche(); 
                break;
            case 3:
                return 0;
        }
    }
}
