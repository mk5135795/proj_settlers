#pragma once

#include <vector>
#include <string>

using namespace std;

struct parameter
{
    int power;
    int chance;
};

struct pos
{
    int x;
    int y;
};

struct stats
{
    string id;
    int lv;
    int exp;
    int hp;
    int mp;
    int vit;
    int str;
    int dex;
    int inte;
    int agi;
};

class Settl
{
private:
    float  _per;
    int    _per_type;
    int    _lv_up;
    int    _inv[2][2];
    string _log;
    stats  _s;
    pos    _cur;

    int   decide();
    int   advance_decide(bool y);
    void  skill_assign(int n);
    void  experience(int val);
    float power(int enm_agi, int enm_per, int bonus);
    float defend(int bonus);
    float magic(int enm_agi, int enm_per, int bonus);
    void log_add(string str);

    static pos _min, _max;
    static char *sp_log(string txt, string arg_1, float arg_2, string *prime = nullptr);
public:
    Settl();
    bool hurt(float &dmg);
    void mp_regen(int val);
    void lv_up(int exp);
    void place(int x, int y);
    int personality();
    pos position();
    stats s_stats();
    const char *c_stats();
    ~Settl();

    static vector<Settl *> sett_vec;
    static vector<parameter> items;
    static vector<string> names;

    static void init();
    static void  generate(int n, int x_min, int x_max, int y_min, int y_max);
    static Settl *attack(Settl *atck, Settl *dfndr, string *log = nullptr);
    static Settl *move(int n);
    static void  move_all();
    static pos   get_area(bool max_min);
    static void  set_area(int x_min, int x_max, int y_min, int y_max);
};
