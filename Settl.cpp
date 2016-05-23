/*

per = 4 * per * (1 - per);
tmp = 4 * tmp * (1 - tmp);
x = 75*tmp*(cos(tmp-per));// *-1 + 100
y = 50*per*(cos(per-tmp));
x += (x >=50) ? (50-x)/2 : (50-x)/2;// - 25 37
y += (x >=50) ? (50-x)/2 : (50-x)/2;// - 2  51

att_old *= 100 - ((rnd - 50) * (rnd - 50) / (chance / 10));
att *= 100 - ((x - 50) * (x - 50) / (chance(1 - 100)));
per = 4 * per * (1 - per);    ->    int(per * 100)
per = 4 * per * (1 - per);
tmp = 4 * tmp * (1 - tmp);
x = 60*tmp*(cos(tmp-per));// *-1 + 100
y = 60*per*(cos(per-tmp));// *-1 + 100


1      rational     2
  1               2
    1           2
      1       2
        1   2
introvert 0 extrovert
        4   3
      4       3
    4           3
  4               3
4     emotional     3

1 - blue
2 - red
3 - yellow
4 - green

passive
introvert +def
extrovert +str
rational  +int
emotional +agi

active
1
analyze -       analyze enemy's next move and choose best counterattack
precision -     +15% chance to attack/defend
2
determined -    10% chance of evading deadly attack                                  //% chance to revive with 20% hp
ambitious -     +10% dmg
3
impulsive -     10% chance to surprise attack (enemy cant def if enemy.agi < agi)
mobile -        +20% chance to evade
4
patient -       +10%dmg after guarding                                               //defend till enemy drop guard, +10%dmg when attacking
                +10% def

magic
fire -      extrovert
ice -       introvert
thunder -   rational
heal -      emotional

*/
#include "Settl.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iterator>
#include <math.h>
#include <string>
#include <time.h>

#define BASE_HP 55

using namespace std;

vector  <string>   Settl::names;
vector <parameter> Settl::items;
vector  <Settl *>  Settl::sett_vec;
pos Settl::_min = {0, 0};
pos Settl::_max = {0, 0};

Settl::Settl(){
	static int amountOfNames = (init(), names.size());

    _per   = (rand() % 99 + 1) / 100.;
    _s.id  = names[ decide() % amountOfNames ];
    _s.lv  = _s.vit = _s.str = _s.dex = _s.inte = _s.agi = 1;
    _s.exp = _lv_up = 0;
    _per_type = decide() % 4;
    skill_assign(15);
    _s.hp  = (BASE_HP + _s.vit * 5) * 100;
    _s.mp  = _s.inte * 15;
    _cur.x = _cur.y = 0;
    _inv[0][0] = _inv[1][0] = _inv[0][1] = _inv[1][1] = 0;
    return;
}

int Settl::decide(){
    float tmp;
     tmp = _per * 100;
    _per = 3.9 * _per * (1 - _per);
    return int(tmp);
}

int Settl::advance_decide(bool y){
    float tmp(_per + 0.01), a;

    _per = 3.9 * _per * (1 - _per);
     tmp = 3.9 *  tmp * (1 -  tmp);
    if(y) a =  60 *  tmp * cos( tmp - _per);
    else  a =  60 * _per * cos(_per -  tmp);
    if( (y && (_per_type == 1 || _per_type == 2)) || (!y && _per_type == 3 || _per_type == 2) )
    {
        a *= -1;
        a += 100;
    }
    return a;
}

void Settl::skill_assign(int n){
    int p;
    while(n-- > 0)
    {
        p = decide();
        if(p <= 25)
        {
            if(_per_type > 1)      _s.agi++;
            else                   _s.inte++;
        }
        else if(p <= 40)//15
        {
            if(_per_type % 3 == 0) _s.dex++;
            else                   _s.str++;
        }
        else if(p <= 60)//20
        {
            _s.vit++;
        }
        else if(p <= 75)//15
        {
            if(_per_type > 1)      _s.inte++;
            else                   _s.agi++;
        }
        else//25
        {
            if(_per_type % 3 == 0) _s.str++;
            else                   _s.dex++;
        }
    }
    return;
}

void Settl::experience(int val){
    if(_s.lv + _lv_up >= 100)
        return;
    _s.exp += val;
    while( _s.exp >= int( pow(2, 4 + _s.lv + _lv_up) ) )
    {
        _s.exp -= int(pow(2, 4 +_s.lv));
        _lv_up++;
    }
    return;
}

float Settl::power(int enm_agi, int enm_per, int bonus){
    float attack, tmp(0);
    attack = _s.str + items[ _inv[0][0] ].power * _inv[0][1];//basic str + weapon_power * weapon_lv
    tmp = items[_inv[0][0]].chance + 15 + 5 * _inv[0][1];//chance
    if(enm_per == 2)//mobile
        tmp -= enm_agi * 1.2;
    else
        tmp -= enm_agi;
    tmp = pow( (decide() - 50), 2 ) / tmp;
    if(_per_type == 0)//precision
        tmp *= 1.15;
    attack *= (100 - tmp) / 100;
    if(_per_type == 1)
        attack *= 1.1;
    if(bonus % 2 == 0)
        attack *= 1.1;
    if(attack < 0)
        attack = 0;
    return attack;
}

float Settl::defend(int bonus){
    float attack;
    attack = _s.dex * items[ _inv[1][0] ].power * _inv[1][1];
    attack *= 1 - (pow( (decide() - 50), 2 ) / (items[ _inv[1][0] ].chance + 15 + 2 * _inv[1][1])) / 100;
    return attack;
}

float Settl::magic(int enm_agi, int enm_per, int bonus){
    float attack;
    //att[0] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte                                                                     ) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
    //att[0] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte - 1.2 * stt[1]->s_stats().agi * ((stt[1]->_per_type == 2) ? 1.2 : 1)) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
    return attack;
}

void Settl::log_add(string str){
    _log += str;
    return;
}


bool Settl::hurt(float &dmg){
    _s.hp -= dmg * 100;
    if(_s.hp <= 0)
    {
        dmg  += _s.hp / 100;
        _s.hp = 0;
        if(_per_type == 1 && rand() % 100 < 10)
        {
            _s.hp += dmg;
            return false;
        }
        return true;
    }
    else if(_s.hp > (BASE_HP + _s.vit * 5) * 100 && dmg < 0)
    {
        dmg  -= _s.hp / 100 - (BASE_HP + _s.vit * 5);
        _s.hp = (BASE_HP + _s.vit * 5) * 100;
    }
    return false;
}

void Settl::mp_regen(int val){
    _s.mp += val;
    if( _s.mp > _s.inte * 15 )
        _s.mp = _s.inte * 15;
    return;
}

void Settl::lv_up(int exp){
    experience(exp);
    if(_lv_up)
    {
        _s.lv += _lv_up;
        skill_assign(4 * _lv_up);
        _lv_up = 0;
        _s.hp  = (BASE_HP + _s.vit * 5) * 100;
        _s.mp  = _s.inte * 15;
    }
    return;
}

void Settl::place(int x, int y){
	_cur.x = x;
	_cur.y = y;
    return;
};

int Settl::personality(){
    return _per_type;
};

pos Settl::position(){
    return _cur;
};

stats Settl::s_stats(){
    return _s;
}

const char *Settl::c_stats(){
    string str;
    str = "\n" + _s.id
        + "\t\tLv: " + to_string(_s.lv) + ((_lv_up == 0) ? "" : "*")
        + "[" + to_string(_s.exp) + "/" + to_string(int(pow(2, 4 +_s.lv + _lv_up)))
        + "]\tType: " + ((_per_type == 3) ? "IV" : ((_per_type == 2) ? "III" : ((_per_type == 1) ? "II" : "I")))
        + "\nHp: "  + to_string(_s.hp / 100) + "." + to_string(_s.hp % 100) + "/" + to_string(BASE_HP + _s.vit * 5)
        + "\tMp: "  + to_string(_s.mp)       + "/" + to_string(_s.inte * 15)
        + "\nVit: " + to_string(_s.vit)
        + "\nStr: " + to_string(_s.str)
        + "\nDex: " + to_string(_s.dex)
        + "\nInt: " + to_string(_s.inte)
        + "\nAgi: " + to_string(_s.agi) + "\n";
    return str.c_str();
}

Settl::~Settl(){
    string fname("log\\");
    ofstream out;
    fname += to_string(sett_vec.size()) + " " + _s.id + ".txt";
    out.open(fname);
    out << "##################################################\n" << c_stats() << "\n##################################################\n\n" << _log;
    return;
}



char *Settl::sp_log(string txt, string arg_1, float arg_2, string *prime){
    char tmp[75];

    if(txt.size() > 30)         txt = "error";
    if(arg_1.size() > 20)       arg_1 = "too_long_name";
    if(arg_2 < 0)               arg_2 *= -1;
    if(int(arg_2) / 100000 > 0) arg_2 = 99999.99;

    sprintf(tmp, txt.c_str(), arg_1.c_str(), arg_2);
    if(prime)
        *prime += tmp;
    return tmp;
};

void Settl::init(){
    ifstream file("names.dat");
    parameter tmp;
	copy(istream_iterator<string>(file),
		istream_iterator<string>(),
		back_inserter(names));
	file.close();
	for(int i(2); i < 10; i++)
    {
        tmp.chance = floor(i / 2) * 10 - 10;
        tmp.power =  floor((i + 18) / 2) - (i % 2) * 3;
        items.push_back(tmp);
    }
    _min.x = _min.y = 0;
    _max.x = _max.y = 100;
    return;
};

void Settl::generate(int n, int x_min, int x_max, int y_min, int y_max){
    bool tmp(false);
    Settl *ptr;
    for(int i(0), x(0), y(0); i < n; i++, tmp = false)
    {
        ptr = new Settl();
        set_area(x_min, x_max, y_min, y_max);
        while(!tmp)
        {
            tmp = true;
            x = rand() % (x_max - x_min) + x_min;
            y = rand() % (y_max - y_min) + y_min;
            for(int i_2(0); i_2 < i; i_2++)
            {
                if(sett_vec[i_2]->position().x == x && sett_vec[i_2]->position().y == y)
                {
                    tmp = false;
                    break;
                }
            }
        }
        ptr->place(x, y);
        sett_vec.push_back(ptr);
    }
    return;
};

Settl *Settl::attack(Settl *atck, Settl *dfndr, string *log){
    int action[2] = {}, magic[2], bonus[2];
    float att[2];
    char buffer[100];
    Settl *stt[2]= {atck, dfndr};
    if(log) *log = "";
    for(int turn(1); turn < 15; turn++)
    {
        bonus[0] = bonus[1] = 1;
        for(int i(0); i < 2; i++)
        {
            stt[i]->mp_regen(stt[i]->s_stats().mp / 2);
            if(stt[i]->decide() + float(stt[i]->s_stats().inte + stt[i]->s_stats().agi) * stt[i]->s_stats().mp / (stt[i]->s_stats().inte * 15) <= stt[i]->decide() + stt[i]->s_stats().str + stt[i]->s_stats().dex)
            {
                if(stt[i]->decide() >= (stt[i]->s_stats().str * 100. / (stt[i]->s_stats().str + stt[i]->s_stats().dex) ))
                {
                    if(action[i] == 1 && stt[0]->_per_type == 1)
                        bonus[i] *= 2;
                    if(stt[i]->personality() == 2 && stt[i]->s_stats().agi * 1.2 > stt[1 - i]->s_stats().agi && rand() % 100 < 20)
                        bonus[i] *= 3;
                    action[i] = 0;//attack
                }
                else
                {
                    att[i] = stt[i]->defend(bonus[i]);
                    /*att[i] = stt[i]->s_stats().dex * items[ stt[i]->_inv[1][0] ].power * stt[i]->_inv[1][1];
                    att[i] *= 1 - (pow( (stt[i]->decide() - 50), 2 ) / (items[ stt[i]->_inv[1][0] ].chance + 15 + 2 * stt[i]->_inv[1][1])) / 100;
                    if(att[i] < 0)
                        att[i] = 0;*/
                    action[i] = 1;//defend
                }
            }
            else
            {
                if(stt[i]->decide() >= (stt[i]->s_stats().inte * 100. / (stt[i]->s_stats().inte + stt[i]->s_stats().agi) ))
                {
                    if(stt[i]->decide() > stt[i]->s_stats().hp / (BASE_HP + stt[i]->s_stats().vit * 5) && stt[i]->s_stats().inte > 7 && stt[i]->s_stats().mp >= 10 + 2 * stt[i]->s_stats().inte)
                    {
                        magic[i] = 0;
                        stt[i]->mp_regen(-10 - 3 * stt[i]->s_stats().inte);
                    }
                    else if(stt[i]->s_stats().mp >= 10 + 2 * stt[i]->s_stats().inte)
                    {
                        magic[i] = 1;
                        stt[i]->mp_regen(-10 - 2 * stt[i]->s_stats().inte);
                    }
                    else
                        magic[i] = 3;
                    action[i] = 2;//magic
                }
                else
                    action[i] = 3;//evade
            }
        }
        if(log) *log += "\n";
        switch(action[0])
        {
        case 0:
            switch(action[1])
            {
            case 0:
                att[0] = stt[0]->power(0, stt[1]->personality(), bonus[0]);
                att[1] = stt[1]->power(0, stt[0]->personality(), bonus[1]);
                for(int i(0), tmp(att[0] + att[1] + 1); i < 2; i++)
                {
                    if(bonus[i] % 3 != 0)
                        att[i] *= att[i] / tmp;
                    stt[1 - i]->hurt(att[i]);
                    stt[i]->experience(att[i]);
                    sp_log("%s deal %.2f damage\n", stt[i]->s_stats().id.c_str(), att[i], log);
                }
                break;
            case 1:
                att[0] = stt[0]->power(0, stt[1]->personality(), bonus[0]);
                if(att[1] > att[0])
                    att[1] = att[0];
                if(bonus[0] % 3 == 0)
                    att[1] = 0;
                else
                    att[0] -= att[1];
                stt[1]->hurt(att[0]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s deal %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s block %.2f damage\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 2:
                att[0] = stt[0]->power(0, stt[1]->personality(), bonus[0]);
                att[1] = stt[1]->s_stats().inte * (100 - (pow( (stt[1]->decide() - 50), 2 ) / ((15 + stt[1]->s_stats().inte) * ( (stt[1]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[1] < 0)
                    att[1] = 0;
                if(magic[1] == 1)
                    stt[0]->hurt(att[1]);
                else if(magic[1] == 0)
                {
                    att[1] *= -1;
                    stt[1]->hurt(att[1]);
                }
                else
                    att[1] = 0;
                stt[1]->hurt(att[0]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s deal %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                if(magic[1] == 0)
                    sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[1]->_s.id.c_str(), att[1], log);
                break;
            case 3:
                att[0] = stt[0]->power(stt[1]->s_stats().agi, stt[1]->personality(), bonus[0]);
                att[1] = stt[0]->power(0, stt[1]->personality(), bonus[0]) - stt[0]->power(stt[1]->s_stats().agi, stt[1]->personality(), bonus[0]);
                stt[1]->hurt(att[0]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s deal %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s evade %.2f damage\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            }
            break;
        case 1:
            switch(action[1])
            {
            case 0:
                att[1] = stt[1]->power(0, stt[0]->personality(), bonus[1]);
                if(att[0] > att[1])
                    att[0] = att[1];
                if(bonus[1] % 3 == 0)
                    att[0] = 0;
                else
                    att[1] -= att[0];
                stt[0]->hurt(att[1]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s deal %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s block %.2f damage\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 1:
                att[0] = -1;
                att[1] = -1;
                stt[0]->hurt(att[0]);
                stt[1]->hurt(att[1]);
                sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 2:
                att[1] = stt[1]->s_stats().inte * (100 - (pow( (stt[1]->decide() - 50), 2 ) / ((15 + stt[1]->s_stats().inte) * ( (stt[1]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[1] < 0)
                    att[1] = 0;
                if(magic[1] == 1)
                {
                    if(att[0] > att[1])
                        att[0] = att[1];
                    att[1] -= att[0] * 0.8;
                    stt[0]->hurt(att[1]);
                }
                else if(magic[1] == 0)
                {
                    att[1] *= -1;
                    stt[1]->hurt(att[1]);
                    att[0] = 0;
                }
                else
                {
                    att[0] = 0;
                    att[1] = 0;
                }
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s block %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                if(magic[1] == 0)
                    sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 3:
                att[0] = -1;
                att[1] = -1;
                stt[0]->hurt(att[0]);
                stt[1]->hurt(att[1]);
                sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            }
            break;
        case 2:
            switch(action[1])
            {
            case 0:
                att[1] = stt[1]->power(0, stt[0]->personality(), bonus[1]);
                att[0] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[0] < 0)
                    att[0] = 0;
                if(magic[0] == 1)
                    stt[1]->hurt(att[0]);
                else if(magic[0] == 0)
                {
                    att[0] *= -1;
                    stt[0]->hurt(att[0]);
                }
                else
                    att[0] = 0;
                stt[0]->hurt(att[1]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                if(magic[0] == 0)
                    sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s deal %.2f damage\n",  stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 1:
                att[0] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[0] < 0)
                    att[0] = 0;
                if(magic[0] == 1)
                {
                    if(att[1] > att[0])
                        att[1] = att[0];
                    att[0] -= att[1] * 0.8;
                    stt[1]->hurt(att[0]);
                }
                else if(magic[0] == 0)
                {
                    att[0] *= -1;
                    stt[0]->hurt(att[0]);
                    att[1] = 0;
                }
                else
                {
                    att[0] = 0;
                    att[1] = 0;
                }
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                if(magic[0] == 0)
                    sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s block %.2f damage\n",  stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 2:
                for(int i(0); i < 2; i++)
                {
                    att[i] = stt[i]->s_stats().inte * (100 - (pow( (stt[i]->decide() - 50), 2 ) / ((15 + stt[i]->s_stats().inte) * ( (stt[i]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                    if(att[i] < 0)
                        att[i] = 0;
                    if(magic[i]  == 1)
                        stt[1 - i]->hurt(att[i]);
                    else if(magic[i] == 0)
                    {
                        att[i] *= -1;
                        stt[i]->hurt(att[i]);
                    }
                    else
                        att[i] = 0;
                    stt[i]->experience(att[i]);
                    if(magic[i] == 0)
                        sp_log("%s heal %.2f hp\n", stt[i]->s_stats().id.c_str(), att[i], log);
                    else
                        sp_log("%s deal %.2f magic damage\n", stt[i]->s_stats().id.c_str(), att[i], log);
                }
                break;
            case 3:
                att[1] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[1] < 0)
                    att[1] = 0;
                if(15 + stt[0]->s_stats().inte - 1.2 * stt[1]->s_stats().agi > 0)
                    att[0] = stt[0]->s_stats().inte * (100 - (pow( (stt[0]->decide() - 50), 2 ) / ((15 + stt[0]->s_stats().inte - 1.2 * stt[1]->s_stats().agi * ((stt[1]->_per_type == 2) ? 1.2 : 1)) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                else
                    att[0] = 0;
                if(att[0] < 0)
                    att[0] = 0;
                if(magic[0] == 1)
                    stt[1]->hurt(att[0]);
                else if(magic[0] == 0)
                {
                    att[0] *= -1;
                    stt[0]->hurt(att[0]);
                    att[1] = 0;
                }
                else
                {
                    att[0] = 0;
                    att[1] = 0;
                }
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                if(magic[0] == 0)
                    sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s evade %.2f damage\n",  stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            }
            break;
        case 3:
            switch(action[1])
            {
            case 0:
                att[1] = stt[1]->power(stt[0]->s_stats().agi, stt[0]->personality(), bonus[1]);
                att[0] = stt[1]->power(0, stt[0]->personality(), bonus[1]) - stt[1]->power(stt[0]->s_stats().agi, stt[0]->personality(), bonus[1]);
                stt[0]->hurt(att[1]);
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                sp_log("%s evade %.2f damage\n",  stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s deal %.2f damage\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 1:
                att[0] = -1;
                att[1] = -1;
                stt[0]->hurt(att[0]);
                stt[1]->hurt(att[1]);
                sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 2:
                att[0] = stt[1]->s_stats().inte * (100 - (pow( (stt[1]->decide() - 50), 2 ) / ((15 + stt[1]->s_stats().inte) * ( (stt[0]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                if(att[0] < 0)
                    att[0] = 0;
                if(15 + stt[1]->s_stats().inte - 1.2 * stt[0]->s_stats().agi > 0)
                    att[1] = stt[1]->s_stats().inte * (100 - (pow( (stt[1]->decide() - 50), 2 ) / ((15 + stt[1]->s_stats().inte - 1.2 * stt[0]->s_stats().agi * ((stt[0]->_per_type == 2) ? 1.2 : 1)) * ( (stt[1]->_per_type == 1) ? 1.15 : 1) ))) / 50;
                else
                    att[1] = 0;
                if(att[1] < 0)
                    att[1] = 0;
                if(magic[1] == 1)
                    stt[0]->hurt(att[1]);
                else if(magic[1] == 0)
                {
                    att[1] *= -1;
                    stt[1]->hurt(att[1]);
                    att[0] = 0;
                }
                else
                {
                    att[0] = 0;
                    att[1] = 0;
                }
                stt[0]->experience(att[0]);
                stt[1]->experience(att[1]);
                if(magic[0] == 0)
                    sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                else
                    sp_log("%s deal %.2f magic damage\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s evade %.2f damage\n",  stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            case 3:
                att[0] = -1;
                att[1] = -1;
                stt[0]->hurt(att[0]);
                stt[1]->hurt(att[1]);
                sp_log("%s heal %.2f hp\n", stt[0]->s_stats().id.c_str(), att[0], log);
                sp_log("%s heal %.2f hp\n", stt[1]->s_stats().id.c_str(), att[1], log);
                break;
            }
            break;
        }
        if(stt[0]->s_stats().hp <= 0)
        {
            stt[1]->lv_up(10 * stt[0]->s_stats().lv);
            sp_log("\n%s died\n", stt[0]->s_stats().id.c_str(), 0, log);
            sp_log("%s win with %.2f hp\n", stt[1]->s_stats().id.c_str(), stt[1]->s_stats().hp / 100., log);
            if(stt[1]->s_stats().hp <= 0)
            {
                att[1] = -1;
                stt[1]->hurt(att[1]);
            }
            return stt[0];
        }
        else if(stt[1]->s_stats().hp <= 0)
        {
            stt[0]->lv_up(10 * stt[1]->s_stats().lv);
            sp_log("\n%s win with %.2f hp\n", stt[0]->s_stats().id.c_str(), stt[0]->s_stats().hp / 100., log);
            sp_log("%s died\n", stt[1]->s_stats().id.c_str(), 0, log);
            return stt[1];
        }
    }
    sp_log("\n%s end with %.2f hp\n", stt[0]->s_stats().id.c_str(), stt[0]->s_stats().hp / 100., log);
    sp_log("%s end with %.2f hp\n", stt[1]->s_stats().id.c_str(), stt[1]->s_stats().hp / 100., log);
    stt[0]->lv_up(0);
    stt[1]->lv_up(0);
    return nullptr;
}

Settl *Settl::move(int n){
    int x(0), y(0);
    float t((BASE_HP + sett_vec[n]->s_stats().vit * 5) * 10);

    if(get_area(false).x == get_area(true).x
    && get_area(false).y == get_area(true).y)
        return nullptr;

    if(     sett_vec[n]->position().x >= get_area(true).x - 1)
        x = -1;
    else if(sett_vec[n]->position().x <= get_area(false).x)
        x = 1;
    if(     sett_vec[n]->position().y >= get_area(true).y - 1)
        y = -1;
    else if(sett_vec[n]->position().y <= get_area(false).y)
        y = 1;
    if(!x && !y)
    {
        int tmp;

        t = -1 * sett_vec[n]->s_stats().vit;
        tmp = (sett_vec[n]->decide() + sett_vec[n]->advance_decide(false)) % 100;
        if(     tmp < int((sett_vec[n]->position().x - get_area(false).x) * 100 / (get_area(true).x - get_area(false).x)))
            x = -1;
        else if(tmp > int((sett_vec[n]->position().x - get_area(false).x) * 100 / (get_area(true).x - get_area(false).x)))
            x = 1;
        tmp = (sett_vec[n]->decide() + sett_vec[n]->advance_decide(true)) % 100;
        if(     tmp < int((sett_vec[n]->position().y - get_area(false).y) * 100 / (get_area(true).y - get_area(false).y)))
            y = -1;
        else if(tmp > int((sett_vec[n]->position().y - get_area(false).y) * 100 / (get_area(true).y - get_area(false).y)))
            y = 1;
    }
    sett_vec[n]->hurt(t);
    for(int i(0); i <= sett_vec.size(); i++)
    {
        if(i == n) continue;
        if(i == sett_vec.size())
        {
            sett_vec[n]->place(sett_vec[n]->position().x + x,
                               sett_vec[n]->position().y + y);
        }
        else if(sett_vec[i]->position().x == sett_vec[n]->position().x + x
             && sett_vec[i]->position().y == sett_vec[n]->position().y + y)
        {
            string tmp;
            sett_vec[n]->log_add(sett_vec[i]->c_stats());
            sett_vec[i]->log_add(sett_vec[n]->c_stats());
            Settl *ptr = attack(sett_vec[n], sett_vec[i], &tmp);
            sett_vec[n]->log_add(tmp);
            sett_vec[i]->log_add(tmp);
            if(ptr == sett_vec[i])
            {
                sett_vec[n]->place(sett_vec[n]->position().x + x,
                                   sett_vec[n]->position().y + y);
                sett_vec.erase(sett_vec.begin() + i);
                delete ptr;
            }
            else if(ptr == sett_vec[n])
            {
                sett_vec.erase(sett_vec.begin() + n);
                delete ptr;
            }
            return ptr;
        }
    }
    return nullptr;
};

void Settl::move_all(){
    static st_turn(0);
    Settl *tmp;

    st_turn++;
    if(st_turn >= 100)
    {
        st_turn = 90;
        set_area(get_area(false).x + 1, get_area(true).x - 1,
                 get_area(false).y + 1, get_area(true).y - 1);
        if((get_area(false).x >= get_area(true).x) || (get_area(false).y >= get_area(true).y))
        {
            set_area(get_area(false).x, get_area(true).x + 1,
                     get_area(false).y, get_area(true).y + 1);
            st_turn = 101;
        }
    }
    for(int i_1(0); i_1 < sett_vec.size(); i_1++)
    {
        for(int i_2(0); i_2 < sett_vec.size(); i_2++)
        {
            if(sett_vec[i_1]->s_stats().agi < sett_vec[i_2]->s_stats().agi)
            {
                tmp = sett_vec[i_1];
                sett_vec[i_1] = sett_vec[i_2];
                sett_vec[i_2] = tmp;
            }
        }
    }
    for(int i(0); i < sett_vec.size(); i++)
        move(i);
    return;
}

void Settl::set_area(int x_min, int x_max, int y_min, int y_max){
    _min.x = x_min;
	_min.y = y_min;
	_max.x = x_max;
	_max.y = y_max;
    return;
};

pos Settl::get_area(bool max_min){
    return ((max_min) ? _max : _min);
};
