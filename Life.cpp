#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>

#define NO_PARENT 0

using namespace std;

class Life;

int id_creator();
int random_int(int init, int end);
void generate_grass();
void print_grid();

void move_cursor(int x, int y);

const int Grid_x = 20;
const int Grid_y = 80;

int grid[Grid_x][Grid_y];

pair<int, int> head_rot[4] = { {1, 0}, {0, 1}, {-1, 0},{0, -1} };
char head_img[4] = {'v', '>', '^', '<'};
int grass_id = 1;
char white_space = ' ';
vector <int> ids;

int min_Gene = 1;
int max_Gene = 5;
vector<Life> pop;

struct Gene {
	int speed;
	int vision;
	int smell;
	int id;
};


class Life {
public:

	int x, y, head_x, head_y, orientation;
	int metabolism, energy;
	bool alive;

	Gene animal_gene = {
		1,
		1,
		1,
		1
	};

	void kill() {
		move_cursor(x, y);
		cout << white_space;
		grid[x][y] = 0;

		move_cursor(head_x, head_y);
		cout << white_space;
		grid[head_x][head_y] = 0;
		
		x = 0;
		y = 0;
		head_x = 0;
		head_y = 0;
		alive = false;
	}

	Life(int start_x, int start_y, Gene parent_gene) {

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
			if (animal_gene.vision > max_Gene * 2) {
				animal_gene.vision--;
			}
			if (animal_gene.smell > max_Gene) {
				animal_gene.smell--;
			}
		}

		x = start_x;
		y = start_y;

		grid[start_x][start_y] = parent_gene.id;

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
			}
			if (i == 4) grid[start_x][start_y] = 0;
		}

		energy = 100;
		metabolism = animal_gene.speed * 1 + animal_gene.smell * 0 + animal_gene.vision * 0;

		move_cursor(x, y);
		cout << '#';
		move_cursor(head_x, head_y);
		cout << head_img[orientation];

		alive = true;
		cout << flush;
	}

	int see_food() {

		int vision = animal_gene.vision;
		
		for (int i = 0; i < vision; i++) {
			
			int check_x = head_x + i * head_rot[orientation].first;
			int check_y = head_y + i * head_rot[orientation].second;
			
			if (check_x < 0 || check_x >= Grid_x || check_y < 0 || check_y >= Grid_y) {
				return 0;
			}
			else if (grid[check_x][check_y] == grass_id) {
				return 1;
			}
		}

		return 0;
	}

	int smell_food() {
		return 0;
	}

	void rot_mov(bool first, bool past_value) {
		
		bool mov;
		if (first) mov = random_int(0, 1);
		else mov = !past_value;
		
		int new_orientation;

		if (mov) {
			new_orientation = (orientation + 1) % 4;
		}
		else {
			new_orientation = (orientation + 3) % 4;
		}


		int new_x = x + head_rot[new_orientation].first;
		int new_y = y + head_rot[new_orientation].second;

		// check limits
		if (new_x < 0 || new_x >= Grid_x || new_y < 0 || new_y >= Grid_y) {
			rot_mov(false, mov);
			return;
		}
		// check other animals
		if (grid[new_x][new_y] == 0) {

		}
		else if (grid[new_x][new_y] == grass_id) {
			energy += 100;
		}
		else {
			rot_mov(false, mov);
			return;
		}

		grid[head_x][head_y] = 0;
		move_cursor(head_x, head_y);
		cout << white_space;

		grid[new_x][new_y] = -1 * animal_gene.id;
		move_cursor(new_x, new_y);
		cout << head_img[new_orientation];

		orientation = new_orientation;
		head_x = new_x;
		head_y = new_y;
	}

	void linear_mov() {
		
		int new_x = head_x + head_rot[orientation].first;
		int new_y = head_y + head_rot[orientation].second;

		// check limits
		if (new_x < 0 || new_x >= Grid_x || new_y < 0 || new_y >= Grid_y) {
			rot_mov(true, 0);
			return;
		}

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
			rot_mov(true, 0);
		}
		else {
			linear_mov();
		}
	}
	
	

	int moviment_control(int num_moviment) {

		// if the num of moviments is greater than the actual moviments
		if (num_moviment > animal_gene.speed) return 1;


		int front_x = head_x + head_rot[orientation].first;
		int front_y = head_y + head_rot[orientation].second;
		
		// eat grass
		if (front_x >= 0 && front_x < Grid_x && front_y >= 0 && front_y < Grid_y){

			if(grid[front_x][front_y] == grass_id) {
					
				grid[front_x][front_y] = 0;
				energy += 100;

				if (energy >= 200) {
					pair<int,int> pos_animal = head_rot[random_int(1, 4)] ;
					
					Life new_animal(x + pos_animal.first, y + pos_animal.second, animal_gene);
					pop.push_back(new_animal);
					energy -= 100;
					
					return 1;
				}
			}
		}

		// priority cases
		if (see_food()) {
			linear_mov();
		}

		else if (smell_food()) {
			return 1;
		}
		else {
			random_mov();
		}
		
		energy -= metabolism;
		if (energy <= 0) {
			kill();
			return 0;
		}
		return 1;
		cout << flush;
	}
	
};

void update_dashboard(vector<Life>& animal);


int main() {

	ios_base::sync_with_stdio(0);

	print_grid();
	generate_grass();
	

	Gene no_parent = {
		NO_PARENT,
		NO_PARENT,
		NO_PARENT,
		NO_PARENT
	};


	Life animal_1(10, 10, no_parent);
	Life animal_2(10, 30, no_parent);
	Life animal_3(15, 50, no_parent);
	Life animal_4(8, 2, no_parent);
	
	pop.push_back(animal_1);
	pop.push_back(animal_2);
	pop.push_back(animal_3);
	pop.push_back(animal_4);
	
	update_dashboard(pop);
	cin.get();

	while (true) {
		
		for (int j = 1; j <= max_Gene; j++) {
			for (int i = 0; i < pop.size(); i++) {

				
				if (pop[i].moviment_control(j) == 0) {
					pop.erase(pop.begin() + i);
				}
				update_dashboard(pop);
				move_cursor(Grid_x + 2, 0);

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

			if (value == 10 && grid[i][j] == 0) {
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
				continue;
			}

			if (grid[i][j] == 1) cout << 'W';

			else if (grid[i][j] < 0) {
				continue;
				//cout << head_img[pop[(abs(grid[i][j]))].orientation];
			}
			else cout << grid[i][j];
		}
		cout << "\n";
	}
}

void update_dashboard(vector<Life>& animal) {
	
	for (int i = 0; i < animal.size(); i++) {
		
		move_cursor(i * 7 + 1, Grid_y + 5);
		cout << "--- Status Animal " << animal[i].animal_gene.id << " ---";
		
		move_cursor(i * 7 + 2, Grid_y + 5);
		cout << "Speed: " << animal[i].animal_gene.speed;
		
		move_cursor(i * 7 + 3, Grid_y + 5);
		cout << "Vision: " << animal[i].animal_gene.vision;

		move_cursor(i * 7 + 4, Grid_y + 5);
		cout << "Smell: " << animal[i].animal_gene.smell;


		// energy
		move_cursor(i * 7 + 5, Grid_y + 5);
		cout << "                     ";
		move_cursor(i * 7 + 5, Grid_y + 5);
		cout << "Energy: " << animal[i].energy;

		// pos x and y
		move_cursor(i * 7 + 6, Grid_y + 5);
		cout << "                     ";
		move_cursor(i * 7 + 6, Grid_y + 5);
		cout << "x: " << animal[i].x << " " << "y: " << animal[i].y;
	}
}

void move_cursor(int x, int y) {
	cout << "\33[" << (x + 1) << ";" << (y + 1) << "H";
}