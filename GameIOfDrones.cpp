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

using namespace std;


int P; // number of players in the game (2 to 4 players)
int ID; // ID of your player (0, 1, 2, or 3)
int D; // number of drones in each team (3 to 11)
int Z; // number of zones on the map (4 to 8)
int FREEDRONE; // number of free allied drone





class Vector2
{
public:
    float x;
    float y;
    
    Vector2(int xValue, int yValue) : x(xValue), y(yValue){}
    
    int Distance(const Vector2 & v) const {return sqrt(((x - v.x) * (x - v.x)) + ((y - v.y) * (y - v.y))); }
    
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
private:

    
    int maxDistApproaching = 0;

public:
    multimap<int, Drone*> mapFoeDroneByDist;
    multimap<int, Drone*> mapFriendDroneByDist;
    multimap<int, Drone*> mapDroneByDist;
    multimap<int, Drone*> mapFriendAndApprochingDroneByDist;
    
    int id;
    Vector2 pos;

    int teamId = -1;
    
    int approachingByTeam[4];
    vector<int> nbOfDroneByTeamId = {0,0,0,0};
    int backUpNeeded = -1;
    float moveValue = -1;
    

    Zone(int xValue, int yValue, int idValue) : pos(xValue, yValue), id(idValue){}
    
    string debug()
    {
        return "teamId = " + to_string(teamId) + " x = " + to_string(pos.x) + " y = " + to_string(pos.y); 
    }
    
    void ReInit(int tid) // each turn
    {
        memset(approachingByTeam, 0, sizeof approachingByTeam);
        maxDistApproaching = 0;
        moveValue = -1;
        nbOfDroneByTeamId = {0,0,0,0};
        teamId = tid;
        mapFriendAndApprochingDroneByDist.clear();
        setNearestDrone();
    }
    
    void CheckIfAproaching(Drone &d, Vector2 newpPos) 
    {
        if (IntersectionRayCircle(d.pos, newpPos - d.pos, pos, 99))
        {
            int distance;
            if ((distance= pos.Distance(d.pos)) > maxDistApproaching)
                maxDistApproaching = distance;
            approachingByTeam[d.teamId]++;
            mapFriendAndApprochingDroneByDist.insert(pair<int, Drone*>(distance, &d));
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
        mapFriendAndApprochingDroneByDist.clear();
        mapFoeDroneByDist.clear();
        for (int i = 0; i < 4; i++)
        {
            for(Drone &d : drone[i])
            {
                mapDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
                if (d.teamId == ID)
                {
                    mapFriendAndApprochingDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
                    mapFriendDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
                }
                else
                    mapFoeDroneByDist.insert(pair<int, Drone*>(pos.Distance(d.pos), &d));
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
    
    int  calcFoeBackUpUsingMap(multimap<int, Drone*> map)
    {
        int actualOwner = teamId;
        bool OwnerJustchange = false;
        bool tryingTooReturn = false;
        int renfortneeded = 0;
        int nbFoeByTeam[4] = {0,0,0,0};
        int max = 0;
        int maxFoe = 0;
        for (const pair<int, Drone*> &p : map)
        {
            Drone *d = p.second;
            
            if (d->getReserved() && d->targetId != id)
                continue ;

            nbFoeByTeam[d->teamId]++;
             OwnerJustchange = false;

            if (max < nbFoeByTeam[d->teamId])
            {
                max = nbFoeByTeam[d->teamId];
                if (d->teamId)
                    maxFoe = max;
                if (p.first > 99)
                {
                    if (actualOwner != d->teamId)
                        OwnerJustchange = true;
                        
                    actualOwner = d->teamId;
                    
                    if (OwnerJustchange && ID == d->teamId)
                        renfortneeded = max;
                }
                else if (d->teamId != ID)
                    renfortneeded = maxFoe + ((actualOwner == ID) ? 0 : 1);
            }
            if (actualOwner == ID && p.first >= maxDistApproaching)
            {
                if (calcFoeInDist(p.first + 200) == maxFoe)
                    return renfortneeded - approachingByTeam[ID];
            }
        }
        return renfortneeded - approachingByTeam[ID];
    }
    


    int SimulateTeam(const multimap<int, Drone*> &map, int limitFriend)
    {
        int actualOwner = teamId;
        int nbFoeByTeam[4] = {0,0,0,0};
        int max = 0;
        for (const pair<int, Drone*> &p : map)
        {
            Drone *d = p.second;
            // cerr << "team: " << d->teamId << " targetId: " << d->targetId << endl;
            if (d->teamId == ID && d->targetId != id)
                continue ;
            nbFoeByTeam[d->teamId]++;

            if (max < nbFoeByTeam[d->teamId])
            {
                max = nbFoeByTeam[d->teamId];
                if (p.first > 99)
                    actualOwner = d->teamId;
            }
            if (p.first > 99 && nbFoeByTeam[ID] >= limitFriend)
                break ;
        }
        // cerr << endl << endl;
        return actualOwner;
    }
    
    bool EvaluateMove(bool calcFoeBackup) try
    {
            
        setNearestFriendDrone();
        
        backUpNeeded = calcFoeBackUpUsingMap(mapDroneByDist);
        
        if (backUpNeeded > 0)
        {
            float maxDist = getDistInMapDroneByIndex(backUpNeeded, mapDroneByDist);
            moveValue = 100000 - backUpNeeded * maxDist;
        }
        // printMapDroneByDist();
        cerr  << "id: " << id << " teamId: " << teamId << " backUpNeeded: " << backUpNeeded << " Friend approaching: " << approachingByTeam[ID] << " calcFoeBackup: " << calcFoeBackup << " FREEDRONE " << FREEDRONE << endl;
        return backUpNeeded > 0 && (FREEDRONE - backUpNeeded) > 0;
    } catch (const std::exception& e)
    {
        cerr << e.what();
        return -1;
    }
    
    bool MakeMove()
    {
        int i = 0;
        for (multimap<int, Drone*>::iterator p = mapFriendDroneByDist.begin(); p != mapFriendDroneByDist.end(); ++p)
        {
            Drone *d = p->second;
            if (i >= backUpNeeded)
                break ;
            if (d->getReserved() == false)
            {
                approachingByTeam[ID]++;
                d->targetId = id;
                i++;
                d->target = pos;
                d->setReservedToTrue();
            }
        }
        cerr << "id: " << id << " teamId: " << teamId << " friend approching: " << approachingByTeam[ID] << " needed: " << backUpNeeded << endl;
        moveValue = -1;
    }
    
    void setAproaching(int backUpNeeded)
    {
        int i = 0;
        for (multimap<int, Drone*>::iterator p = mapFriendDroneByDist.begin(); p != mapFriendDroneByDist.end(); ++p)
        {
            Drone *d = p->second;
            // cerr << "1team: " << d->teamId << " targetId: " << d->targetId << endl;
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
                // originalPossibility.totalDroneLeft[1] = notComplete.front().totalDroneLeft[1];
                // for (int l = 0; 0 <= originalPossibility.totalDroneLeft[1]; l++)
                // {
                //     originalPossibility.zoneBackup[i][1] = l;
                //     originalPossibility.totalDroneLeft[2] = notComplete.front().totalDroneLeft[2];
                //     for (int m = 0; 0 <= originalPossibility.totalDroneLeft[2]; m++)
                //     {
                //         originalPossibility.zoneBackup[i][2] = m;
                //         originalPossibility.totalDroneLeft[3] = notComplete.front().totalDroneLeft[3];
                //         for (int n = 0; 0 <= originalPossibility.totalDroneLeft[3]; n++)
                //         {
                //             originalPossibility.zoneBackup[i][3] = n;
                            
                            Possibility newPossibility = originalPossibility;
                            if (newPossibility.IsComplete())
                                possibilityList.push_back(originalPossibility);
                            else
                                notComplete.push_back(newPossibility);
                //             originalPossibility.totalDroneLeft[3]--; 
                //         }
                //         originalPossibility.totalDroneLeft[2]--; 
                //     }
                //     originalPossibility.totalDroneLeft[1]--; 
                // }
                originalPossibility.totalDroneLeft[ID]--; 
            }
            // originalPossibility.CalcAlliedMoveHash();
            notComplete.pop_front();
        }
        cerr << i << " " << possibilityList.size() << endl;
    }
    for (Possibility &p: possibilityList)
        p.CalcNbTarget();
     cerr << possibilityList.size() << endl;

}


void SetTargetidForPossibility(Possibility & p)
{
    for (Drone &d : drone[ID])
        d.targetId = -1;
    for (Zone *z : zone)
        z->setAproaching(p.zoneBackup[z->id][ID]);
}

bool lol = false;

int TestFriendPossibility(Possibility & testing) // we searche the worst case scenario (vector iterate opti ?)
{
    // int scoreFinale = 99999;
    
    
    // vector<multimap<int, Drone*>> friendMap;
    // friendMap.resize(Z);
    // for (int i = 0; i < D; i++)
    //      friendMap[testing.droneCible[i]].insert(pair<int, Drone*>(zone[testing.droneCible[i]].pos.Distance(testing.drone[i]->pos), testing.drone[i]));
    
    
    // for (Possibility & foeTesting : foePossibilityList)
    // {
    //     int score = 0;
       
    //     vector<multimap<int, Drone*>> map = friendMap;

    //     int size = D * (P - 1);
    //     for (int i = 0; i < size; i++)
    //         map[foeTesting.droneCible[i]].insert(pair<int, Drone*>(zone[i].pos.Distance(foeTesting.drone[i]->pos), foeTesting.drone[i]));
            
    //     for (Zone &z : zone)
    //         score += (int)(z.SimulateTeam(map[z.id]) == ID);

    //     if (score < scoreFinale)
    //         scoreFinale = score;
    // }
    // testing.score = scoreFinale;
    
    int score = 0;
    SetTargetidForPossibility(testing);
                  
    for (Zone *z : zone)
        score += (int)(z->SimulateTeam(z->mapDroneByDist, testing.zoneBackup[z->id][ID]) == ID);

    testing.score = score;
    return score;
}

void BruteForce()
{
    lol = false;
    // SearchBestMove
    
    Possibility *best = &possibilityList.front();
    int i = 0;
    cerr << possibilityList.size() << endl;
    TestFriendPossibility(*best); // opti duplicate
    setTargetToTargetId();
    // showq(friendPossibilityList);
    for (Possibility &testing : possibilityList)
    {
        TestFriendPossibility(testing);
        if (testing.score > best->score || (testing.score == best->score && testing.nbTarget > best->nbTarget))
        {
            best = &testing;
            setTargetToTargetId();
        }
        // cerr << "score: " << testing.score << " cible: " << testing.Debug() << endl;
    }
    
    //Setup BestMove
    
    cerr << "score: " << best->score << " cible: " << best->Debug() << endl;

}



void EvaluateAllZone(bool checkingFooBackup = false)
{
    bool moveLeft = true;
    
    while(moveLeft)
    {
        moveLeft = false;
        Zone *bestZone = NULL;
        for(Zone *z : zone)
        {
            if (z->EvaluateMove(checkingFooBackup))
            {
                moveLeft = true;
                if (!bestZone || z->moveValue > bestZone->moveValue)
                    bestZone = z;
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
       
        cerr << "finish" << endl;
            
        for (int i = 0; i < D; i++) // moving drone
        {
            
            Vector2 target = drone[ID][i].target;

            // output a destination point to be reached by one of your drones. The first line corresponds to the first of your drones that you were provided as input, the next to the second, etc.
            cout << to_string((int)target.x) << " " << to_string((int)target.y) << endl;
        }
    }
}
