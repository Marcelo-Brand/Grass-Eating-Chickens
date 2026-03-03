#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>
#include <windows.h>
#include <string>

#define NO_PARENT 0

using namespace std;
using std::cout;
using std::cin;

class Life;
struct Gene;

int id_creator();
int random_int(int init, int end);
void generate_grass();
void print_grid();
void move_cursor(int x, int y);
void update_dashboard();
void generate_animals(int num);
bool check_lim(int x, int y);
void hide_cursor();

const int Grid_x = 30;
const int Grid_y = 80;

int grid[Grid_x][Grid_y];
vector<Life> pop;
pair<int, int> head_rot[4] = { {1, 0}, {0, 1}, {-1, 0},{0, -1} };

// image config
char head_img[4] = {'v', '>', '^', '<'};
int grass_id = 1;
char white_space = ' ';
char body_char = '#';
vector <int> ids;

// inernal and external coonfig
int min_Gene = 1;
int max_Gene = 5;
int grass_energy = 100;
int grass_spaw_prob = 20;

// metabolism calculation
int Vision_weight = 0;
int Speed_weight = 0;
int Smell_weight = 0;
int	Coeficient_of_existence = 0;


struct Gene {
	int speed;
	int vision;
	int smell;
	int id;
};


class Life {
public:
  
	int x = 0, y = 0, head_x = 0, head_y = 0, orientation = 0, energy = 0, metabolism = 0;
    

    Gene animal_gene = {
        1,
        1,
        1,
        1
    };

    Life(int start_x, int start_y, Gene parent_gene) {


		//  verify superposition of body
		int rand = random_int(1, 4);
		if (parent_gene.speed == NO_PARENT) {
			
			for (int i = 0; i < 4; i++) {

				int new_x = start_x + head_rot[(i + rand) % 4].first;
				int new_y = start_y + head_rot[(i + rand) % 4].second;

				// verify grid violation
				if (new_x >= 0 && new_x < Grid_x && new_y >= 0 && new_y < Grid_y) {

					if (grid[new_x][new_y] == 0) {
						start_x = new_x;
						start_y = new_y;
						break;
					}
					else if (grid[new_x][new_y] == grass_id) {

						energy += grass_energy;
						start_x = new_x;
						start_y = new_y;
						break;
					}

				}

				if (i == 3) {
					return;
				}
			}

		
		}
		


		// if has no parent
		if (parent_gene.speed == NO_PARENT) {
			animal_gene = {
				random_int(min_Gene, max_Gene),
				random_int(min_Gene, max_Gene * 2),
				random_int(min_Gene, max_Gene),
				id_creator()
			};
		}
		// if has parent
		else {
			animal_gene = {
				random_int(-1, 1) + parent_gene.speed,
				random_int(-1, 1) + parent_gene.vision,
				random_int(-1, 1) + parent_gene.smell,
				id_creator()
			};

			if (animal_gene.speed > max_Gene) {
				animal_gene.speed--;
			}
			else if (animal_gene.speed == 0) {
				animal_gene.speed++;
			}
			if (animal_gene.vision > max_Gene * 2) {
				animal_gene.vision--;
			}
			
			if (animal_gene.smell > max_Gene) {
				animal_gene.smell--;
			}

			int random = random_int(0, 3); /*
			int new_x = start_x + [random];
			int new_y = start_y +;*/

			for (int i = 0; i < 4; i++) {
			
				int new_x = start_x + head_rot[(i + random) % 4].first;
				int new_y = start_y + head_rot[(i + random) % 4].second;
				
				if (new_x < 0 || new_x >= Grid_x || new_y < 0 || new_y >= Grid_y) {
					continue;
				}
				
				if (grid[new_x][new_y] == 0) {
					
					start_x = new_x;
					start_y = new_y;
					break;
				}
				else if (grid[new_x][new_y] == grass_id) {
					
					start_x = new_x;
					start_y = new_y;
					energy += grass_energy;
					
					break;
				}

				if (i == 4) {
					return;
				}
			}
		}

		x = start_x;
		y = start_y;

		grid[x][y] = animal_gene.id;

		// creation of the head of the animal with the head with negative of his id
		for (int i = 0; i < 4; i++) {

			int random_index = random_int(1, 4);
			int new_orientation = (i + random_index) % 4;
			pair<int, int> pos = { start_x + head_rot[new_orientation].first, start_y + head_rot[new_orientation].second };

			if ((pos.first >= 0 && pos.first < Grid_x) && (pos.second >= 0 && pos.second < Grid_y)) {

				if (grid[pos.first][pos.second] == 0) {

					grid[pos.first][pos.second] = -1 * parent_gene.id;
					
					head_x = pos.first;
					head_y = pos.second;
					orientation = (i + random_index) % 4;
					
					break;
				}
				// if spaw head in grass eat grass
				else if (grid[pos.first][pos.second] == grass_id) {
					
					energy += grass_energy;
					grid[pos.first][pos.second] = -1 * parent_gene.id;
					
					head_x = pos.first;
					head_y = pos.second;
					orientation = (i + random_index) % 4;
					
					break;
				}
			}
			if (i == 3) grid[x][y] = 0;
			return;
		}

		energy += 150;
		metabolism = animal_gene.speed * Speed_weight + animal_gene.smell * Smell_weight + animal_gene.vision * Vision_weight + Coeficient_of_existence;
		
		pop.push_back(*this);
		
		move_cursor(x, y);
		cout << body_char;

		move_cursor(head_x, head_y);
		cout << head_img[orientation];
		

		cout << flush;
	}

	void kill() {

		move_cursor(x, y);
		cout << white_space;
		grid[x][y] = 0;

		move_cursor(head_x, head_y);
		cout << white_space;
		grid[head_x][head_y] = 0;

		for (int i = 0; i < pop.size(); i++) {
			if (pop[i].animal_gene.id == animal_gene.id) {
				pop.erase(pop.begin() + i);
			}
		}

	}

	void force_rot(string ori) {
		
		energy += grass_energy;
		
		if (ori == "right") {
			orientation = (orientation + 1) % 4;
		}
		else if (ori == "left") {
			orientation = (orientation + 3) % 4;
		}
		else return;

		move_cursor(head_x, head_y);
		cout << white_space;
		grid[head_x][head_y] = 0;



		head_x = x + head_rot[orientation].first;
		head_y = y + head_rot[orientation].second;

		grid[head_x][head_y] = -1 * animal_gene.id;
		move_cursor(head_x, head_y);
		cout << head_img[orientation];
	}

	int see_food() {

		int vision = animal_gene.vision;
		
		
		for (int i = 0; i <= vision; i++) {

			int center_x = x + (i * head_rot[orientation].first);
			int center_y = y + (i * head_rot[orientation].second);

			int rx = center_x + head_rot[(orientation + 1) % 4].first;
			int ry = center_y + head_rot[(orientation + 1) % 4].second;

			int lx = center_x + head_rot[(orientation + 3) % 4].first;
			int ly = center_y + head_rot[(orientation + 3) % 4].second;


			if (check_lim(center_x, center_y)) {

				if (grid[center_x][center_y] == grass_id) return 1;
			}
			
			// if is in the right
			if (check_lim(rx, ry)) {

				if (grid[rx][rx] == grass_id) {

					if (i == 0) {
						force_rot("right");
						return 2;
					}
					else {
						return 1;
					}
				}
			}

			// if is in the left
			if (check_lim(lx, ly)) {

				if (grid[lx][ly] == grass_id) {

					if (i == 0) {
						force_rot("left");
						return 2;
					}
					else {
						return 1;
					}
				}
			}
		}

		return 0;
	}

	int smell_food() const {
		
		
		/*for (int i = 0; i < animal_gene.smell; i++) {
			for (int j = 0; j < animal_gene.smell; j++) {

				if 
			}
		}*/
		return 0;
	}

	void rot_mov() {
		
		bool rand = random_int(0, 1);

		int first_ori, second_ori;

		if (rand) {
			first_ori = (orientation + 1) % 4;
			second_ori = (orientation + 3) % 4;
		}
		else {
			first_ori = (orientation + 3) % 4;
			second_ori = (orientation + 1) % 4;
		}

		int nx = x + head_rot[first_ori].first;
		int ny = y + head_rot[first_ori].second;

		if (check_lim(nx, ny)) {

			if (grid[nx][ny] == 0) {
				
				grid[head_x][head_y] = 0;
				grid[nx][ny] = (-1 * animal_gene.id);
				
				move_cursor(head_x, head_y);
				cout << white_space;

				head_x = nx;
				head_y = ny;
				
				orientation = first_ori;
				
				move_cursor(head_x, head_y);
				cout << head_img[orientation];

				return;
			}
			else if (grid[nx][ny] == grass_id) {

				energy += grass_energy;

				grid[head_x][head_y] = 0;
				grid[nx][ny] = (-1 * animal_gene.id);

				move_cursor(head_x, head_y);
				cout << white_space;

				head_x = nx;
				head_y = ny;

				orientation = first_ori;

				move_cursor(head_x, head_y);
				cout << head_img[orientation];

				return;
			}
		}

		// if the fisrt opitions is locked
		nx = x + head_rot[second_ori].first;
		ny = y + head_rot[second_ori].second;
		
		if (check_lim(nx, ny)) {

			if (grid[nx][ny] == 0) {

				grid[head_x][head_y] = 0;
				grid[nx][ny] = (-1 * animal_gene.id);

				move_cursor(head_x, head_y);
				cout << white_space;

				head_x = nx;
				head_y = ny;

				orientation = second_ori;

				move_cursor(head_x, head_y);
				cout << head_img[orientation];

				return;
			}
			else if (grid[nx][ny] == grass_id) {

				energy += grass_energy;

				grid[head_x][head_y] = 0;
				grid[nx][ny] = (-1 * animal_gene.id);

				move_cursor(head_x, head_y);
				cout << white_space;

				head_x = nx;
				head_y = ny;

				orientation = second_ori;

				move_cursor(head_x, head_y);
				cout << head_img[orientation];
				
				return;
			}
		}
	}

	void linear_mov() {
		
		int new_x = head_x + head_rot[orientation].first;
		int new_y = head_y + head_rot[orientation].second;

		
		// check limits
		if (!check_lim(new_x, new_y)) {
			rot_mov();
			return;
		}

		if (grid[new_x][new_y] == grass_id) energy += grass_energy;

		move_cursor(x, y);
		cout << white_space;
		
		move_cursor(head_x, head_y);
		cout << '#';
		
		move_cursor(new_x, new_y);
		cout << head_img[orientation];

		grid[x][y] = 0;
		grid[head_x][head_y] = animal_gene.id;
		grid[new_x][new_y] = -1 * animal_gene.id;
		// new values after moviment
		x = head_x;
		y = head_y;

		head_x = new_x;
		head_y = new_y;
	}

	void random_mov() {
		int mov_type = random_int(1, 10);

		if (mov_type <= 2) {
			rot_mov();
		}
		else {
			linear_mov();
		}
	}
	
	

	int moviment_control(int num_moviment) {

		// if the num of moviments is greater than the actual moviments
		bool alredy_moved = false;

		if (num_moviment > animal_gene.speed) return 1;

		energy -= metabolism;

		int front_x = head_x + head_rot[orientation].first;
		int front_y = head_y + head_rot[orientation].second;
		
		// eat grass
		if (check_lim(front_x, front_x)){

			// eat grass
			if(grid[front_x][front_y] == grass_id) {
					
				linear_mov();
				alredy_moved = true;
			}

			// avoid animal overlap
			else if (grid[front_x][front_y] != 0) {
				
				rot_mov();
				alredy_moved = true;
			}
		}

		// priority cases
		if (!alredy_moved){
			
			if (see_food()) {
				linear_mov();
			}

			else if (smell_food()) {
				return 1;
			}
			else {
				random_mov();
			}
		}
		
		
		if (energy <= 0) {
			kill();
			return 0;
		}
		else if (energy >= 300) {
			Life(x, y, animal_gene);
			energy -= 150;
			return 2;
		}

		// cout << flush;
		return 1;
	}
};



int main() {

	ios_base::sync_with_stdio(0);

	int num_animals;
	cout << "How many animals: ";
	cin >> num_animals;
	system("cls");
		
	hide_cursor();

	print_grid();
	generate_grass();
	generate_animals(num_animals);

	
	// update_dashboard();
	cin.get();

	while (true) {
		
		for (int j = 1; j <= max_Gene; j++) {
			for (int i = 0; i < pop.size(); i++) {
				
				if (pop[i].moviment_control(j) == 0) {
					i--;
				}
				//update_dashboard();
				move_cursor(Grid_x + 2, 0);
				
				// wait for the next step
				this_thread::sleep_for(chrono::milliseconds(30));
			}
		}
		cin.get();
	}

	return 0;
}

int random_int(int init, int end) {
	static mt19937 gen(random_device{}());
	uniform_int_distribution<int> dis(init, end);
	return dis(gen);
}

void generate_grass() {

	for (int i = 0; i < Grid_x; i++) {
		for (int j = 0; j < Grid_y; j++) {

			int value = random_int(1, 30);

			if (value == 1 && grid[i][j] == 0) {
				grid[i][j] = grass_id;
				move_cursor(i, j);
				cout << 'W';
			}
		}
	}
}

int id_creator() {
	int new_id = ids.size() + 2;
	ids.push_back(new_id);
	return new_id;
}

void print_grid() {

	for (int i = 0; i < Grid_x; i++) {

		for (int j = 0; j < Grid_y; j++) {

			if (grid[i][j] == 0) {
				cout << white_space;
			}
			// print grass
			else if (grid[i][j] == grass_id) cout << 'W';
		}
		cout << "\n";
	}
}

void update_dashboard() {
	
	for (int i = 0; i < pop.size(); i++) {
		
		move_cursor(i * 7 + 1, Grid_y + 5);
		cout << "--- Status Animal " << pop[i].animal_gene.id << " ---";
		
		move_cursor(i * 7 + 2, Grid_y + 5);
		cout << "Speed: " << pop[i].animal_gene.speed;
		
		move_cursor(i * 7 + 3, Grid_y + 5);
		cout << "Vision: " << pop[i].animal_gene.vision;

		move_cursor(i * 7 + 4, Grid_y + 5);
		cout << "Smell: " << pop[i].animal_gene.smell;


		// energy
		move_cursor(i * 7 + 5, Grid_y + 5);
		cout << "                     ";
		move_cursor(i * 7 + 5, Grid_y + 5);
		cout << "Energy: " << pop[i].energy;

		// pos x and y
		move_cursor(i * 7 + 6, Grid_y + 5);
		cout << "                     ";
		move_cursor(i * 7 + 6, Grid_y + 5);
		cout << "x: " << pop[i].x << " " << "y: " << pop[i].y;
	}
	
	move_cursor(pop.size() * 7 + 6, Grid_y + 5);
	cout << "                     " << "\n";
	cout << "                     ";
	
}

void move_cursor(int x, int y) {
	cout << "\33[" << (x + 1) << ";" << (y + 1) << "H";
}

void generate_animals(int num) {

	Gene no_parent = {NO_PARENT, NO_PARENT, NO_PARENT, NO_PARENT};

	for (int i = 0; i < num; i++) {
		int new_x = random_int(0, Grid_x);
		int new_y = random_int(0, Grid_y);

		Life temp(new_x, new_y, no_parent);

	}
}

bool check_lim(int x, int y) {

	if (x >= 0 && x < Grid_x && y >= 0 && y < Grid_y) {
		return 1;
	}
	else {
		return 0;
	}
}

void hide_cursor() {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;

	// Pega as configurações atuais (para não quebrar nada)
	GetConsoleCursorInfo(consoleHandle, &info);

	// O pulo do gato: Define a visibilidade como FALSA
	info.bVisible = FALSE;

	// Aplica a nova configuração
	SetConsoleCursorInfo(consoleHandle, &info);
}