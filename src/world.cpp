// AUTHOR: Juan García Santos
// DATE: 04/10/2021
// EMAIL: alu0101325583@ull.edu.es
// VERSION: 4.0
// SUBJECT: AI
// PRACTICE Nº: 1
// Compilation: make
// Execution: ./simulation
// Especificaciones de la clase mundo

#include "include/world.h"


//Constructor que crea la malla
World::World(int row_, int col_) {

    row = row_, col = col_;
    world.resize(row);
    for(unsigned int i = 0; i < world.size(); i++){
        world[i].resize(col);
    }
    for(int i = 0; i < row; i++) 
        for(int j = 0; j < col; j++) {
            world[i][j].SetState(0);
            world[i][j].SetX(i);
            world[i][j].SetY(j);
        }
}


// Método que añade los obstáculos al mundo
void
World::Obstacle(int obstacle_percentage, int obstacle_type){

    int x, y;
    int obstacle_quantity = (row * col) * obstacle_percentage / 100;

    switch (obstacle_type) {


    case 0:

        srand(time(NULL)); 
        for(int i = 0; i < obstacle_quantity; i++) {
            int random_row = rand()%(world.size() - 0) + 0;
            int random_col = rand()%(world[random_row].size());
            SetWorldState(1, random_row, random_col);
        }
        break;
    


    case 1:
        for(int i = 0; i < obstacle_quantity; i++) {
            do {
                std::cout << "Introduzca la coordenada X del obstaculo " << i+1 << " : ";
                std::cin >> x;
                while ((x < 0) || (x >= row)) {
                    std::cout << "Esa coordenada X no está dentro del mundo previamente definido. Ojito Cuidado" << std::endl;
                    std::cout << "Introduzca una coordenada entre " << 0 << " y " << row << std:: endl;
                std::cin >> x;
                }

                std::cout << "Introduzca la coordenada Y del obstaculo " << i+1 << " : ";
                std::cin >> y;
                while ((y < 0) || ( y >= col)) {
                    std::cout << "Esa coordenada Y no está dentro del mundo previamente definido. Ojito Cuidado" << std::endl;
                    std::cout << "Introduzca una coordenada entre " << 0 << " y " << col << std:: endl;
                    std::cin >> y;
                }
                
            } while (GetWorldState(x, y) == 1); //mientras la casilla ya esté ocupada

            SetWorldState( 1, x, y);
        }

        break;
    
    case 2:
        try {
        std::ifstream input_file("obstacle.txt");
        std::string read, number1, number2;
        unsigned movement = 0;

        if (!input_file.is_open()) throw 0;
        
        while(getline(input_file, read)) {
            if (read[0] != '/' && read[1] != '/') {

            for(; movement < read.size(); movement ++) {
                if(read[movement] != ',') {
                    number1.push_back(read[movement]);
                }
                else{
                    movement ++;
                    break; 
                }
            }
            for(; movement < read.size(); movement ++) {
                number2.push_back(read[movement]);
            } 

            x = stoi(number1);
            y = stoi(number2);
            number1.clear();
            number2.clear();
            movement = 0;

            if((x < 0) || (x >= row) || (y < 0) || ( y >= col)) throw 1;


            SetWorldState( 1 , x, y);
            }
        }          
        input_file.close();

    }
    catch(int& error) {
        switch (error)
        {
        case 0:
            std::cerr<<"No se pudo abrir el archivo - obstacle.txt"<<std::endl;
            exit(0);
            break;
        
        case 1:
            std::cerr<<"Fichero tiene coordenadas mal"<<std::endl;
            exit(0);
            break;
        }
    }

    break;
    }   
}

// Establece posición de origen y destino
void 
World::SetVehicle(int x1, int y1, int x2, int y2) {
    star_ = std::make_pair(x1, y1);
    end_ = std::make_pair(x2, y2);
    world[x1][y1].SetState(3);
    world[x2][y2].SetState(4);
}


// Getter del mundo
int 
World::GetWorldState(int i, int j) {
    return world[i][j].GetState();
}


// Setter del mundo
void 
World::SetWorldState(int state_, int i, int j) {
    world[i][j].SetState(state_);
}


// Imprime el mundo por la terminal y limpia estructuras
void 
World::PrintWorld(void) {

    for(int i = 0; i < row; i++) {
        for(int j = 0; j < col; j++) {

            world[i][j].CleanSons();
            world[i][j].SetOpen(false);
            world[i][j].SetClose(false);

            switch(world[i][j].GetState()) {

                case 0:
                    std::cout<<termcolor::on_white<<" "<<termcolor::reset;
                break;
                case 1:
                    std::cout<<termcolor::on_color<0, 0, 0><<" "<<termcolor::reset;
                break;
                case 2:
                    std::cout<<termcolor::on_green<<" "<<termcolor::reset;
                    world[i][j].SetState(0);
                break;
                case 3:
                    std::cout<<termcolor::on_red<<"X"<<termcolor::reset;
                break;
                case 4:
                    std::cout<<termcolor::on_blue<<">"<<termcolor::reset;
                break;
                case 5:
                    std::cout<<termcolor::on_bright_magenta<<" "<<termcolor::reset;
                    world[i][j].SetState(0);
                break;
            }
        }
        std::cout<<std::endl;
    }
}

// Permite guardar el camino para llegar al destino
void 
World::RebuildPath() {
    SetContadorCamino(0);
    int i = world[end_.first][end_.second].GetDad().first;
    int j = world[end_.first][end_.second].GetDad().second;
    int aux_i, aux_j;
    int contador = 0;

    while(world[i][j].GetDad().first != -1 && world[i][j].GetDad().second != -1) {
        world[i][j].SetState(2);
        aux_i = world[i][j].GetDad().first;
        aux_j = world[i][j].GetDad().second;
        i = aux_i; j = aux_j;
        contador += 1;
    }
    SetContadorCamino(contador + 1);
}

// Creación de hijos tanto en 4 como 8 direcciones
void
World::CreateSons(int x, int y, bool direction) {
    //world[x][y].CleanSons();

    if(x - 1 >= 0 && world[x - 1][y].GetState() != 1)
        world[x][y].SetSons(x - 1, y);
    if(x + 1 < row && world[x + 1][y].GetState() != 1)
        world[x][y].SetSons(x + 1, y);
    if(y - 1 >= 0 && world[x][y - 1].GetState() != 1)
        world[x][y].SetSons(x, y - 1);
    if(y + 1 < col && world[x][y + 1].GetState() != 1)
        world[x][y].SetSons(x, y + 1);

    if(direction) {
        
        if(x - 1 >= 0 && y - 1 >= 0 && world[x - 1][y - 1].GetState() != 1)
            world[x][y].SetSons(x - 1, y -1);
        if(x - 1 >= 0 && y + 1 < col && world[x - 1][y + 1].GetState() != 1)
            world[x][y].SetSons(x - 1, y + 1);
        if(x + 1 < row && y - 1 >= 0 && world[x + 1][y - 1].GetState() != 1)
            world[x][y].SetSons(x + 1, y - 1);
        if(x + 1 < row && y + 1 < col && world[x + 1][y + 1].GetState() != 1)
            world[x][y].SetSons(x + 1, y + 1);
    }
}



bool
World::Astar(f_heuristica* heuristica, bool direction){

    SetContadorNodos(0);
    int contador = 0;
    std::list<std::pair<int, int>> open, close; // Lista donde almacenamos los hijos
    
    world[star_.first][star_.second].SetG(0);
    world[star_.first][star_.second].SetF((*heuristica)(world[star_.first][star_.second], world[end_.first][end_.second]));

    std::pair<int, int> current {star_.first, star_.second}; // Guardamos la posicion del nodo actual
    open.push_front(current);
    world[star_.first][star_.second].SetOpen(true);


    while(!open.empty()) { //Mientras lista este llena

        std::list<std::pair<int, int>>::iterator min = open.begin(), it; // Recorremos la lista  buscando el nodo con menor F
        for (it = min; it != open.end(); it++)
            if (world[(*it).first][(*it).second].GetF() < world[(*min).first][(*min).second].GetF())
                min = it;

        current = (*min);

        if((world[(*min).first][(*min).second].GetX() == end_.first) && (end_.second == world[(*min).first][(*min).second].GetY())) { //Hemos llegado al destino
            RebuildPath();
            SetContadorNodos(contador);
            return true;
        }

        world[(*min).first][(*min).second].SetClose(true); // El elemento sera guardado en lista cerrada
        world[(*min).first][(*min).second].SetOpen(false);
        open.erase(min);
        world[(*min).first][(*min).second].SetState(5);
        contador +=1;
        close.push_front(current);

        CreateSons(current.first, current.second, direction); // Creo a los hijos

        for(int i = 0; i < world[current.first][current.second].GetSonsSize(); i++) { // Para cada hijo ...
            int son_x = world[current.first][current.second].GetSons(i).first, son_y = world[current.first][current.second].GetSons(i).second;
            
            if(world[son_x][son_y].GetOpen())  // El elemnto que quiero añadir ya esta en lista abierta?
                if(world[son_x][son_y].GetG() > world[current.first][current.second].GetG() + 1) {
                            world[son_x][son_y].SetG(world[current.first][current.second].GetG() + 1);
                            world[son_x][son_y].SetF((*heuristica)(world[son_x][son_y], world[end_.first][end_.second]) + world[current.first][current.second].GetG() + 1);
                            world[son_x][son_y].SetDad(current.first, current.second);
                }

            if(world[son_x][son_y].GetClose()) // El elemnto que quiero añadir ya esta en lista cerrada?
                if(world[son_x][son_y].GetG() > world[current.first][current.second].GetG()+1)
                    for (std::list<std::pair<int, int>>::iterator aux_it = close.begin(); aux_it != close.end(); aux_it++)
                        if(world[current.first][current.second].GetSons(i).first == current.first && world[current.first][current.second].GetSons(i).second == current.second) {
                            close.erase(aux_it);
                            world[son_x][son_y].SetClose(false);
                            world[son_x][son_y].SetOpen(true);
                            world[son_x][son_y].SetG(world[current.first][current.second].GetG() + 1);
                            world[son_x][son_y].SetF((*heuristica)(world[son_x][son_y], world[end_.first][end_.second]) + world[current.first][current.second].GetG() + 1);
                            world[son_x][son_y].SetDad(current.first, current.second);
                            std::pair<int, int> aux {son_x, son_y};
                            open.push_front(aux);
                            break;
                        }

            if(!world[son_x][son_y].GetClose() && !world[son_x][son_y].GetOpen()) { // El elemtento no esta en ninguna lista
                world[son_x][son_y].SetOpen(true);
                world[son_x][son_y].SetG(world[current.first][current.second].GetG()+1);
                world[son_x][son_y].SetF((*heuristica)(world[son_x][son_y], world[end_.first][end_.second]) + world[current.first][current.second].GetG()+1);
                world[son_x][son_y].SetDad(current.first, current.second);
                std::pair<int, int> aux {son_x, son_y};
                open.push_front(aux);      
            }
        }
    }
    SetContadorNodos(contador);
    return false;
}