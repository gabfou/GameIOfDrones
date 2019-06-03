#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <map>
#include <stdexcept>
#include <cstring>
#include <queue> 
#include <limits>
#include <list>;
#include <chrono>

using namespace std;


int P; // number of players in the game (2 to 4 players)
int ID; // ID of your player (0, 1, 2, or 3)
int D; // number of drones in each team (3 to 11)
int Z; // number of zones on the map (4 to 8)
int FREEDRONE; // number of free allied drone

chrono::time_point<std::chrono::steady_clock> maxTime; // the time before you have too end the turn

class Vector2
{
public:
    float x;
    float y;
    
    Vector2(int xValue, int yValue) : x(xValue), y(yValue){}
    
    int Distance(const Vector2 & v) const {return sqrt(((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y))); }
    int DistanceSqrt(const Vector2 & v) const {return (((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y))); }
    
    float Length() const { return sqrt(x * x + y * y); }
    float LengthSquared() const { return x * x + y * y; }
    void Set(int xValue, int yValue){ x = xValue; y = yValue; }
    string Debug(){return "x=" + to_string(x) + " y=" + to_string(y);}
    
    Vector2 & Normal() { Set(-y, x); return *this; }
    Vector2 & Normalize()
    {
        if(float length = LengthSquared() != 0)
        {
            x /= length; y /= length;
            return *this;
        }
        x = y = 0;
        return *this;
    }
    inline float Dot(const Vector2 & v) const { return x * v.x + y * v.y; }
    
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


bool IntersectionRayCircle(Vector2 originRay, Vector2 dirRay, Vector2 CenterCircle, float rayon)
{
    
    Vector2 idealDir = originRay - CenterCircle;
    
    if (idealDir.Length() < 99)
        return true;
        
    if (dirRay == Vector2(0,0))
        return false;

    
    float a = dirRay.Dot(dirRay);
    float b = 2*idealDir.Dot(dirRay);
    float c = idealDir.Dot(idealDir) - rayon * rayon;
    
    float discriminant = b*b-4*a*c;
    if(discriminant < 0)
    {
        return false;
    }
    else
    {
        // ray didn't totally miss sphere,

        discriminant = sqrt(discriminant);
        
        // either solution may be on or off the ray so need to test both
        float t1 = (-b - discriminant)/(2*a);
        float t2 = (-b + discriminant)/(2*a);

        if (t1 >= 0 || t2 >= 0)
            return true ;
        return false ;
    }
}

class Drone
{
private:
    bool reserved = false;
    
public:
    int id = -1;
    int targetId = -1;
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
        targetId = -1;
    }


};

class Zone;

vector<Drone> drone[4];
vector<Drone*> allDrone;
vector<Zone*> zone;

void  printMap(multimap<int, Drone*> map)
{
    for (const pair<int, Drone*> &p : map)
        cerr << "teamId: " << p.second->teamId << "dist: " << p.first << " pos: (" << p.second->pos.x << "," << p.second->pos.y << ")"<< endl;
}


class Zone
{

public:
    multimap<int, Drone*> mapFriendDroneByDist;
    multimap<int, Drone*> mapDroneByDist;
    
    int id;
    Vector2 pos;

    int teamId = -1;
    int maxDrone;
    
    vector<int> nbOfDroneByTeamId = {0,0,0,0};
    

    Zone(int xValue, int yValue, int idValue) : pos(xValue, yValue), id(idValue){}
    
    string debug()
    {
        return "teamId = " + to_string(teamId) + " x = " + to_string(pos.x) + " y = " + to_string(pos.y); 
    }
    
    void ReInit(int tid) // each turn
    {
        nbOfDroneByTeamId = {0,0,0,0};
        teamId = tid;
        setNearestDrone();
        maxDrone = 0;
    }
    
    void CheckIfAproaching(Drone &d, Vector2 newpPos) 
    {
        if (IntersectionRayCircle(d.pos, newpPos - d.pos, pos, 99))
        {
            if (d.targetId == -1 || d.pos.DistanceSqrt(zone[d.targetId]->pos) > d.pos.DistanceSqrt(pos))
                d.targetId = id;
        }
    }

    int getDistInMapDroneByIndex(int index, multimap<int, Drone*> map)
    {
        if (index < 0 || index >= map.size())
            throw out_of_range(string("getDistInMapDroneByIndex bad index: ") + to_string(index) + "\n");
        return next(map.begin(), index)->first;
        
    }
    
    multimap<int, Drone*> vectorToMultimap(vector<Drone> vec)
    {
        multimap<int, Drone*> ret;
        for(Drone &d : vec)
        {   
            if (d.getReserved() == false)
                ret.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
        }
        return ret;
    }

    void setNearestDrone() // rewriute too use the vectorToMultimap function (duplication!!)
    {
        mapDroneByDist.clear();
        for (int i = 0; i < 4; i++)
        {
            for(Drone &d : drone[i])
            {
                int distance = pos.Distance(d.pos);
                mapDroneByDist.insert(pair<int, Drone*>(distance - ((d.teamId == ID) ? 1 : 0), &d));
                if (d.teamId == ID)
                    mapFriendDroneByDist.insert(pair<int, Drone*>(distance, &d));
                if (distance < 100)
                {
                    nbOfDroneByTeamId[d.teamId]++;
                    if (maxDrone < nbOfDroneByTeamId[d.teamId])
                        maxDrone = nbOfDroneByTeamId[d.teamId];
                }
            }
        }
    }
    

    void setNearestFriendDrone() // 
    {
        mapFriendDroneByDist.clear();
        for(Drone &d : drone[ID])
        {   
            if (d.getReserved() == false)
                mapFriendDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
        }
    }
    
    int  calcDroneOfTeamInDist(int distance, int tid) // not protected
    {
        int nbFoe = 0;
        for(Drone &d : drone[tid])
        {   
                if (d.pos.Distance(pos) <= distance)
                    nbFoe++;
        }
        return nbFoe;
    }


    int  calcFoeInDist(int distance)
    {
        int max = 0;
        int tmp;

        for(int i = 0; i < 4; i++)
        {
            if (i != ID && (tmp = calcDroneOfTeamInDist(distance, i)) > max)
                max = tmp;
        }
        return max;
    }

    int SimulateTeam(const multimap<int, Drone*> &map, int limitFriend, int &addDist)
    {
        int actualOwner = teamId;
        int pre99VirtualOwner = teamId;
        int nbFoeByTeam[4] = {0,0,0,0};
        int max = 0;
        for (const pair<int, Drone*> &p : map)
        {
            Drone *d = p.second;
            if (d->teamId == ID && d->targetId != id)
                continue ;
            if (nbFoeByTeam[ID] >= limitFriend && p.first > 99 && d->targetId != id)
                continue ;
            nbFoeByTeam[d->teamId]++;
            if (d->teamId == ID)
                addDist += p.first;

            if (max < nbFoeByTeam[d->teamId])
            {
                max = nbFoeByTeam[d->teamId];
                if (p.first > 99)
                    actualOwner = d->teamId;
                else
                    pre99VirtualOwner = d->teamId;
            }
        }
        if (max > nbFoeByTeam[actualOwner])
            actualOwner = pre99VirtualOwner;
        return actualOwner;
    }
    
    void setAproaching(int backUpNeeded)
    {
        int i = 0;
        for (multimap<int, Drone*>::iterator p = mapFriendDroneByDist.begin(); p != mapFriendDroneByDist.end(); ++p)
        {
            Drone *d = p->second;
            if (i >= backUpNeeded)
                break ;
            if (d->targetId == -1)
            {
                d->targetId = id;
                i++;
            }
            else if (d->targetId != id && zone[d->targetId]->pos.Distance(d->pos) > p->first)
            {
                d->targetId = id;
                i++;
                zone[d->targetId]->setAproaching(1);
            }
        }
    }
    
};


class Possibility
{
public:
    vector<std::array<int, 4>> zoneBackup;
    int alliedMoveHash = 0; // needed too group all possibility where we make the same move;
    int totalDroneLeft[4];
    int nbTarget= 0;
    int score = 0;
    float averageDistance = 0;
    
    Possibility()
    {
        zoneBackup.resize(zone.size());
        int i = -1;
        while (++i < P)
            totalDroneLeft[i] = D;
        i--;
        while (++i < 4)
            totalDroneLeft[i] = 0;
            
    }

    void CalcAlliedMoveHash()
    {
        int i = 0;
        alliedMoveHash = 0;
        for (std::array<int, 4> vec : zoneBackup)
        {
            alliedMoveHash += vec[ID] * pow(D, i);
            i++;
        }
    }
    
    bool IsComplete()
    {
        return (totalDroneLeft[ID] < 1);
    }

    void CalcNbTarget()
    {
        nbTarget = 0;
        for (int i = 0; i < zoneBackup.size(); i++)
            if (zoneBackup[i][ID] > 0)
                nbTarget++;
    }

    string Debug()
    {
        string ret;
        
        for(auto i : zoneBackup)
            ret += to_string(i[0]) + " " + to_string(i[1]) + " " + to_string(i[2]) + " " + to_string(i[3]) + " : ";
        return ret;
    }
};

list<Possibility> possibilityList;

void showl(const list <Possibility> &gq) 
{
    for (Possibility g : gq)
        cerr <<  g.Debug() << endl;
}



void setTargetToTargetId()
{
    for (Drone &d : drone[ID])
    {
        d.setReservedToTrue();
        d.target = zone[d.targetId]->pos;
    }
}



void BruteForceInitQueue() 
{
    list<Possibility> notComplete;
    // init base possibility (where nobody do nothing)
    notComplete.push_back(Possibility());

    // finding all possibility where everyone do something (ie: has a target)
    for (int i = 0; i < Z; i++) 
    {
        int size = notComplete.size();
        for (int j = 0; j < size; j++)
        {
            Possibility originalPossibility = notComplete.front();
            for (int k = 0; 0 <= originalPossibility.totalDroneLeft[ID]; k++)
            {
                originalPossibility.zoneBackup[i][ID] = k;
                            
                Possibility newPossibility = originalPossibility;
                if (newPossibility.IsComplete())
                    possibilityList.push_back(originalPossibility);
                else
                    notComplete.push_back(newPossibility);
                    
                originalPossibility.totalDroneLeft[ID]--; 
            }
            notComplete.pop_front();
        }
        cerr << i << " " << possibilityList.size() << endl;
    }
    for (Possibility &p: possibilityList)
        p.CalcNbTarget();
     cerr << possibilityList.size() << endl;
    // showl(possibilityList);

}


void SetTargetidForPossibility(Possibility & p)
{
    for (Drone &d : drone[ID])
        d.targetId = -1;
    for (Zone *z : zone)
        z->setAproaching(p.zoneBackup[z->id][ID]);
}


float TestFriendPossibility(Possibility & testing) // we searche the worst case scenario (vector iterate opti ?)
{
    
    float score = 0;
    int averageDistance = 0;
    SetTargetidForPossibility(testing);
                  
    for (Zone *z : zone)
    {
        int result = (int)(z->SimulateTeam(z->mapDroneByDist, testing.zoneBackup[z->id][ID], averageDistance) == ID);
        score += result;
        if (result == 0 && z->maxDrone < z->nbOfDroneByTeamId[ID] + testing.zoneBackup[z->id][ID])
            score += 0.1f;
    }
        
    testing.averageDistance = averageDistance / D;

    testing.score = score;
    return score;
}

void BruteForce()
{
    int i = 0;
    Possibility *best = &possibilityList.front();
    cerr << possibilityList.size() << endl;
    TestFriendPossibility(*best); // opti duplicate
    if (best->score >= Z / P)
        setTargetToTargetId();
        
    for (Possibility &testing : possibilityList)
    {
        TestFriendPossibility(testing);
        if (i == 0)
            testing.nbTarget = 20000;
        if (testing.score > best->score || (testing.score == best->score && (testing.nbTarget > best->nbTarget || (testing.nbTarget == best->nbTarget && testing.averageDistance < best->averageDistance))))
        {
            cerr << "score: " << best->score << " nbTarget: " << best->nbTarget << " cible: " << best->Debug() << endl;
            best = &testing;
            if (testing.score >= Z / P)
                setTargetToTargetId();
        }
        i++;
        if (chrono::steady_clock::now() > maxTime)
        {
            cerr << " Not enought time nb Possibility tested: " << i << endl;
            break ;
        }
    }
    
    cerr << "score: " << best->score << " nbTarget: " << best->nbTarget << " cible: " << best->Debug() << endl;
    // setting the best at the front of the list too give him some priority
    Possibility tmp = *best;
    *best = possibilityList.front();
    possibilityList.front() = tmp;
    

}



/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    
    cerr << "init" << endl;
    Vector2 centerOfAllZone = Vector2(0,0);
    
    cin >> P >> ID >> D >> Z; cin.ignore();
    for (int i = 0; i < Z; i++) {
        int X; // corresponds to the position of the center of a zone. A zone is a circle with a radius of 100 units.
        int Y;
        
        cin >> X >> Y; cin.ignore();
        zone.push_back(new Zone(X,Y, i));
        centerOfAllZone += Vector2(X, Y);
    }
    centerOfAllZone /= Z;
    for (int i = 0; i < P; i++)
    {
        for (int j = 0; j < D; j++)
        {
            drone[i].push_back(Drone(i));
            allDrone.push_back(&drone[i][j]);
        }
    }
    cerr << "initBruteForce" << endl;
    
    BruteForceInitQueue();
    
    cerr << "initEnd" << endl;
    
    // game loop
    while (1) {
        
        maxTime = std::chrono::steady_clock::now() + chrono::milliseconds(95);
        
        FREEDRONE = D;
        
        for (int i = 0; i < Z; i++) // init zone
        {
            int TID; // ID of the team controlling the zone (0, 1, 2, or 3) or -1 if it is not controlled. The zones are given in the same order as in the initialization.
            cin >> TID; cin.ignore();
            zone[i]->ReInit(TID);
           
        }
        
        for (int i = 0; i < P; i++) // init drone and update zone control
        {
            for (int j = 0; j < D; j++)
            {
                int DX; // The first D lines contain the coordinates of drones of a player with the ID 0, the following D lines those of the drones of player 1, and thus it continues until the last player.
                int DY;
                cin >> DX >> DY; cin.ignore();
                if (i != ID)
                {
                    for (Zone *z : zone)
                        z->CheckIfAproaching(drone[i][j], Vector2(DX, DY)); //opti toDo duplicate Vector2(DX, DY)
                }
                drone[i][j].ReInit(Vector2(DX, DY));

            }
        }
        
        cerr << "evaluate" << endl;
        
        
        BruteForce();
        
        
        cerr << "assign do nothing" << endl;
        
        for(Drone &d : drone[ID]) // asign a target to drone who do nothing
        {
            if (d.getReserved() == false)
            {
                d.setReservedToTrue();
                Zone *zNear = NULL;
                for (Zone *z : zone)
                {
                    if (z->teamId != ID && (!zNear || z->pos.Distance(d.pos) < zNear->pos.Distance(d.pos)))
                        zNear = z;
                }
                if (zNear)
                    d.target = zNear->pos;
                else
                    d.target = centerOfAllZone;
            }
        }
        cerr << "finish" << endl;
            
        for (int i = 0; i < D; i++) // moving drone
        {
            
            Vector2 target = drone[ID][i].target;

            // output a destination point to be reached by one of your drones. The first line corresponds to the first of your drones that you were provided as input, the next to the second, etc.
            cout << to_string((int)target.x) << " " << to_string((int)target.y) << endl;
        }
    }
}
