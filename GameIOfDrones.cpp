#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <map>
#include <stdexcept>
#include <cstring>

using namespace std;


    int P; // number of players in the game (2 to 4 players)
    int ID; // ID of your player (0, 1, 2, or 3)
    int D; // number of drones in each team (3 to 11)
    int Z; // number of zones on the map (4 to 8)
    int FREEDRONE; // number of free allied drone



class Vector2
{
public:
    int x;
    int y;
    
    Vector2(int xValue, int yValue) : x(xValue), y(yValue){}
    
    int Distance(const Vector2 & v) const {return sqrt(((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y))); }
    
    //ASSINGMENT AND EQUALITY OPERATIONS
    inline Vector2 & operator = (const Vector2 & v) { x = v.x; y = v.y; return *this; }
    inline Vector2 & operator = (const int & f) { x = f; y = f; return *this; }
    inline Vector2 & operator - (void) { x = -x; y = -y; return *this; }
    inline bool operator == (const Vector2 & v) const { return (x == v.x) && (y == v.y); }
    inline bool operator != (const Vector2 & v) const { return (x != v.x) || (y != v.y); }
    
    //VECTOR2 TO VECTOR2 OPERATIONS
    inline const Vector2 operator + (const Vector2 & v) const { return Vector2(x + v.x, y + v.y); }
    inline const Vector2 operator - (const Vector2 & v) const { return Vector2(x - v.x, y - v.y); }
    inline const Vector2 operator * (const Vector2 & v) const { return Vector2(x * v.x, y * v.y); }
    inline const Vector2 operator / (const Vector2 & v) const { return Vector2(x / v.x, y / v.y); }
    
    //VECTOR2 TO THIS OPERATIONS
    inline Vector2 & operator += (const Vector2 & v) { x += v.x; y += v.y; return *this; }
    inline Vector2 & operator -= (const Vector2 & v) { x -= v.x; y -= v.y; return *this; }
    inline Vector2 & operator *= (const Vector2 & v) { x *= v.x; y *= v.y; return *this; }
    inline Vector2 & operator /= (const Vector2 & v) { x /= v.x; y /= v.y; return *this; }
    
    //SCALER TO VECTOR2 OPERATIONS
    inline const Vector2 operator + (int v) const { return Vector2(x + v, y + v); }
    inline const Vector2 operator - (int v) const { return Vector2(x - v, y - v); }
    inline const Vector2 operator * (int v) const { return Vector2(x * v, y * v); }
    inline const Vector2 operator / (int v) const { return Vector2(x / v, y / v); }
    
    //SCALER TO THIS OPERATIONS
    inline Vector2 & operator += (int v) { x += v; y += v; return *this; }
    inline Vector2 & operator -= (int v) { x -= v; y -= v; return *this; }
    inline Vector2 & operator *= (int v) { x *= v; y *= v; return *this; }
    inline Vector2 & operator /= (int v) { x /= v; y /= v; return *this; }
};


class Drone
{
private:
    bool reserved = false;
    
public:
    Vector2 pos;
    int teamId = -1;
    Vector2 target = Vector2(0,0);
    
    bool getReserved(){return reserved;}
    void setReservedToTrue(){if (reserved) return; reserved = true;FREEDRONE--;}
    

    Drone(int teamIdValue) : pos(0, 0), teamId(teamIdValue){}    
    Drone(int xValue, int yValue, int teamIdValue) : pos(xValue, yValue){}
    
    void ReInit(Vector2 newPos) // each turn
    {
        pos = newPos;
        target = pos;
        reserved = false;
    }


};

vector<Drone> drone[4];

class Zone
{
private:

    int aproachingByTeam[4];
    multimap<int, Drone*> mapDroneByDist;

public:

    Vector2 pos;

    int id = -1;
    
    vector<int> nbOfDroneByTeamId = {0,0,0,0};
    int backUpNeeded = -1;
    float moveValue = -1;
    bool moveMade = false;
    

    Zone(int xValue, int yValue) : pos(xValue, yValue){}
    
    string debug()
    {
        return "id = " + to_string(id) + " x = " + to_string(pos.x) + " y = " + to_string(pos.y); 
    }
    
    void ReInit(int tid) // each turn
    {
        moveMade = false;
        memset(aproachingByTeam, 0, sizeof aproachingByTeam);
        moveValue = -1;
        nbOfDroneByTeamId = {0,0,0,0};
        id = tid;
    }

    inline int getDistInMapDroneByIndex(int index)
    {
        if (index < 0 || index >= mapDroneByDist.size())
            throw out_of_range(string("getDistInMapDroneByIndex bad index: ") + to_string(index) + "\n");
        return next(mapDroneByDist.begin(), index)->first;
        
    }

    void setNearestDrone() // 
    {
        mapDroneByDist.clear();
        for(Drone &d : drone[ID])
        {   
            if (d.getReserved() == false)
                mapDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
        }
    }
    
    void  printMapDroneByDist()
    {
        for (const pair<int, Drone*> &p : mapDroneByDist)
            cerr << "dist: " << p.first << " pos: (" << p.second->pos.x << "," << p.second->pos.y << ")"<< endl;
    }


    int  calcFoeBackUpInDist(int distance)
    {
        int max = 0;
        for(int i = 0; i < 4; i++)
        {
            int nbFoe = 0;
            if(i == ID)
                continue ;
            for(Drone &d : drone[i])
            {   
                if (d.pos.Distance(pos) < distance)
                    nbFoe++;
            }
            if (nbFoe > max)
                max = nbFoe;
        }
        max =  max + ((id == ID) ? 0 : 1);
        return max;
    }
    
    bool EvaluateMove(bool calcFoeBackup) try
    {
        if (moveMade) // second state to protect getDistInMapDroneByIndex delete it later 
            return false;
            
        setNearestDrone();
        
        backUpNeeded = calcFoeBackUpInDist(100);
        if (calcFoeBackup && backUpNeeded > 0) // second state to protect getDistInMapDroneByIndex delete it later 
        {
            while (backUpNeeded <= FREEDRONE)
            {
                int distance = getDistInMapDroneByIndex(backUpNeeded - 1);
                printMapDroneByDist();
                cerr<< "distance : " << distance << endl;
                cerr<< "backUpNeeded - 1 : " << backUpNeeded - 1 << endl;
                int newBackUpNeeded = calcFoeBackUpInDist(distance);
                if (newBackUpNeeded <= backUpNeeded)
                    break ;
                else
                    backUpNeeded = newBackUpNeeded;
            }
        }

        moveValue = FREEDRONE - backUpNeeded;
        cerr << "id: " << id << " backUpNeeded: " << backUpNeeded << " calcFoeBackup: " << calcFoeBackup << endl;
        return moveValue >= 0;
    } catch (const std::exception& e)
    {
        cerr << e.what();
        return -1;
    }
    
    bool MakeMove()
    {
        for (multimap<int, Drone*>::iterator p = mapDroneByDist.begin(); p != mapDroneByDist.end(); ++p)
        {
            Drone *d = p->second;
            if (aproachingByTeam[ID] >= backUpNeeded)
                break ;
            if (d->getReserved() == false)
            {
                aproachingByTeam[ID]++;
                d->target = pos;
                d->setReservedToTrue();
            }
        }
        FREEDRONE -= aproachingByTeam[ID];
        cerr << "id: " << id << " friend approching: " << aproachingByTeam[ID] << " needed: " << backUpNeeded << endl; 
        moveMade = true;
        moveValue = -1;
    }
    
};

vector<Zone> zone;



void EvaluateAllZone(bool checkingFooBackup = false)
{
    bool moveLeft = true;
    
    while(moveLeft)
    {
        moveLeft = false;
        Zone *bestZone = NULL;
        for(Zone &z : zone)
        {
            if (z.EvaluateMove(checkingFooBackup))
            {
                moveLeft = true;
                if (!bestZone || z.moveValue > bestZone->moveValue)
                    bestZone = &z;
            }
        }
        if (bestZone)
            bestZone->MakeMove();
    }
}



/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    
    cin >> P >> ID >> D >> Z; cin.ignore();
    for (int i = 0; i < Z; i++) {
        int X; // corresponds to the position of the center of a zone. A zone is a circle with a radius of 100 units.
        int Y;
        
        cin >> X >> Y; cin.ignore();
        zone.push_back(Zone(X,Y));
    }
    for (int i = 0; i < P; i++)
    {
        for (int j = 0; j < D; j++)
        {
            drone[i].push_back(Drone(i));
        }
    }
    
    // game loop
    while (1) {
        
        FREEDRONE = D;
        
        for (int i = 0; i < Z; i++) // init zone
        {
            int TID; // ID of the team controlling the zone (0, 1, 2, or 3) or -1 if it is not controlled. The zones are given in the same order as in the initialization.
            cin >> TID; cin.ignore();
            zone[i].ReInit(TID);
           
        }
        
        for (int i = 0; i < P; i++) // init drone and update zone control
        {
            for (int j = 0; j < D; j++)
            {
                int DX; // The first D lines contain the coordinates of drones of a player with the ID 0, the following D lines those of the drones of player 1, and thus it continues until the last player.
                int DY;
                cin >> DX >> DY; cin.ignore();
                drone[i][j].ReInit(Vector2(DX, DY));
            }
        }
        
        cerr << "evaluate" << endl;
        
        EvaluateAllZone(true); // evaluate and prepare move while checking foe backup

        
        EvaluateAllZone(false); // evaluate and prepare move without checking foe backup
        
        
        cerr << "assign do nothing" << endl;
        
        for(Drone &d : drone[ID]) // asign a target to drone who do nothing
        {
            if (d.getReserved() == false)
            {
                d.setReservedToTrue();
                Zone *zNear = NULL;
                for (Zone &z : zone)
                {
                    if (z.id != ID && z.moveMade == false && (!zNear || z.pos.Distance(d.pos) < zNear->pos.Distance(d.pos)))
                        zNear = &z;
                }
                if (zNear)
                {
                    zNear->moveMade = true;
                    d.target = zNear->pos;
                }
                // d.target.x = 2000; d.target.y = 900;
            }
        }
        cerr << "finish" << endl;
            
        for (int i = 0; i < D; i++) // moving drone
        {
            
            Vector2 target = drone[ID][i].target;

            // output a destination point to be reached by one of your drones. The first line corresponds to the first of your drones that you were provided as input, the next to the second, etc.
            cout << to_string(target.x) << " " << to_string(target.y) << endl;
        }
    }
}
