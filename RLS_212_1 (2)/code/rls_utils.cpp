
#include<map>
#include<iostream>
#include<vector>

#include"rls_utils.h"
#include"coordsystem.h"


std::map<size_t, std::vector<GeoData>> Coord_converter::converse_to_GeoData(const std::map<size_t, std::vector<Etalon_point>>& inp)
{
    std::map<size_t, std::vector<GeoData>> res;
    for(auto& el : inp)
    {
        std::vector<GeoData> vec_gd;
        double pr_obd = 0; //наклонная дальность на прошлом шагк, нужно для определения знака скорости
        for(auto& d : el.second)
        {

            GeoData gd;
            WGS84 coord;

            gd.mod_time = d.mod_time;
            //gd.cp_id = d.cp_id;
            gd.trace_id = d.trace_id;
            if(inp_param_type.first == "WGS84")
                coord.setGeoD(Point3D(d.latitude, d.longitude, d.height));
            else if(inp_param_type.first == "SK42"){
                SK42 inp_c;
                inp_c.setGeoD(Point3D(d.latitude, d.longitude, d.height));
                coord = inp_c;
            }
            else if(inp_param_type.first == "PZ90_11"){
                PZ90_11 inp_c;
                inp_c.setGeoD(Point3D(d.latitude, d.longitude, d.height));
                coord = inp_c;
            }
            else throw std::runtime_error("wrong CS for traces");

            WGS84 rls_c;

            if(inp_param_type.second == "WGS84")
                rls_c.setGeoD(rls_coord.getP3D());
            else if(inp_param_type.second == "SK42"){
                SK42 inp_c;
                inp_c.setGeoD(rls_coord.getP3D());
                coord = inp_c;
            }
            else if(inp_param_type.second == "PZ90_11"){
                PZ90_11 inp_c;
                inp_c.setGeoD(rls_coord.getP3D());
                coord = inp_c;
            }
            else throw std::runtime_error("wrong CS for rls_coord");



            LRTCS lrtcs(coord, rls_c);
            SpCS sp = lrtcs;
            Point3D sp_c = sp.getSpCS();
            gd.oblique_dist = sp_c.X();
            gd.azim = sp_c.Y();
            gd.aim_angle = sp_c.Z();
            gd.rad_speed = d.speed.mod();
            if(pr_obd > gd.oblique_dist)
                gd.rad_speed *= -1;
            pr_obd = gd.oblique_dist;
            vec_gd.push_back(gd);
        }
        res[el.first] = vec_gd;
    }
    return res;
}
double M_vec::mod() const{
    return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}
std::ostream& operator<<(std::ostream &out, M_vec& mv){
    out << "m_vec: " << mv.x << " " << mv.y << " " << mv.z << std::endl;
    return out;
}

std::ostream& operator<<(std::ostream &out, Etalon_point& ep)
{
    out << "trace_id: " << ep.trace_id << std::endl;
    //out << "cp_id: " << ep.cp_id << std::endl;
    out << "mod_time: " << ep.mod_time << std::endl;
    out << "latitude: " << ep.latitude << std::endl;
    out << "longitude: " << ep.longitude << std::endl;
    out << "height: " << ep.height << std::endl;
    out << "coord: " << std::endl;
    out << ep.coord;
    out << "speed: " << std::endl;
    out << ep.speed;
    out << "accel: " << std::endl;
    out << ep.accel;
    return out;
}


std::ostream& operator<<(std::ostream &out, GeoData& ep)
{
    out << "trace_id: " << ep.trace_id << std::endl;
    out << "cp_id: " << ep.cp_id << std::endl;
    out << "mod_time: " << ep.mod_time << std::endl;
    out << "oblique_dist: " << ep.oblique_dist << std::endl;
    out << "azim: " << ep.azim << std::endl;
    out << "aim_angle: " << ep.aim_angle << std::endl;
    out << "rad_speed: " << ep.rad_speed << std::endl;
    return out;

}
/*void Coord_converter::set_poi_res(const std::map<size_t, std::vector<GeoData>>& inp)
        { poi_result = inp; }*/
void Coord_converter::set_etalon_traces(const std::map<size_t, std::vector<Etalon_point>>& inp)
        { traces = inp; }
void Coord_converter::set_rls_coodr(double x, double y, double z)
    { rls_coord = {x,y,z}; }
void Coord_converter::set_rls_coodr(const M_vec& rls_c)
    { rls_coord = rls_c; }

std::map<size_t, std::vector<GeoData>> Coord_converter::get_etalon_traces_spher()
        { return Coord_converter::converse_to_GeoData(traces); }
/*std::map<size_t, std::vector<GeoData>> Coord_converter::get_poi_res()
        { return poi_result; }*/
M_vec Coord_converter::get_rls_coord()
        { return rls_coord; }

bool Etalon_point::operator<(const Etalon_point &p2) const
        { return this->mod_time < p2.mod_time; }
bool Etalon_point::operator<=(const Etalon_point &p2) const
        { return this->mod_time <= p2.mod_time; }
bool GeoData::operator<(const GeoData &p2) const
        { return this->mod_time < p2.mod_time; }
bool GeoData::operator<=(const GeoData &p2) const
        { return this->mod_time <= p2.mod_time; }

bool is_equal(double x, double y) {
    return std::fabs(x - y) < 0.0002;
}
bool is_equal(const M_vec& v1, const M_vec& v2)
        { return is_equal(v1.x, v2.x) and is_equal(v1.y, v2.y) and is_equal(v1.z, v2.z); }

bool Etalon_point::operator==(const Etalon_point& p2) const // равными считаются при равенстве координат, скоростей и ускорений
{
    return is_equal(latitude, p2.latitude) and is_equal(longitude, p2.longitude) and is_equal(height, p2.height) and
            is_equal(coord, p2.coord) and is_equal(speed, p2.coord) and is_equal(accel, p2.accel);
}

bool GeoData::operator==(const GeoData& p2) const // равными считаются при равенстве координат и скоростей
{
    return is_equal(azim, p2.azim) and is_equal(oblique_dist, p2.oblique_dist) and is_equal(aim_angle, p2.aim_angle) and
            is_equal(rad_speed, p2.rad_speed);
}

Point3D M_vec::getP3D() const
    { return Point3D(x,y,z); }

void Coord_converter::set_inp_par_type(const std::pair<std::string, std::string>& ipt)
    {inp_param_type = ipt;}

GeoData::GeoData(double mt, size_t zn, size_t ci, size_t ti, double az, double od, double aa, double rs):
    mod_time{mt},zond_num{zn}, cp_id{ci}, trace_id{ti}, azim{az}, oblique_dist{od}, aim_angle{aa}, rad_speed{rs}{};
GeoData::GeoData():mod_time{0},zond_num{0}, cp_id{0}, trace_id{0}, azim{0}, oblique_dist{0}, aim_angle{0}, rad_speed{0}{};


std::map<size_t, std::vector<std::pair<double,double>>> Coord_converter::conv_from_sph_to_tpc(std::map<size_t, std::vector<GeoData>> poi) const
{
    std::map<size_t, std::vector<std::pair<double,double>>> res;
    for(auto& vec : poi)
    {
        for(auto& el : vec.second)
        {
            SpCS sp;
            sp.setRlsCoord(Point3D(rls_coord.getP3D()));
            sp.setSpCS(Point3D{el.oblique_dist, el.azim, el.aim_angle});
            LRTCS p;
            p = sp;
            std::pair<double,double> pai;
            pai.first = p.getLRTC().X();
            pai.second = p.getLRTC().Y();
            res[vec.first].push_back(pai);
        }
    }
    return res;
}


std::map<std::size_t, std::vector<Point3D>> Coord_converter::conv_from_tpc3_to_sph(std::map<std::size_t, std::vector<Point3D>> inp) const
{
    std::map<size_t, std::vector<Point3D>> res;
    for(auto& el : inp)
    {

        std::vector<Point3D> vec_gd;
        for(auto& d : el.second)
        {

            LRTCS lrtcs;
            lrtcs.setLRTC(d);
            lrtcs.setRlsCoord(rls_coord.getP3D());
            SpCS sp = lrtcs;
            Point3D sp_c = sp.getSpCS();
            vec_gd.push_back(sp_c);
        }
        res[el.first] = vec_gd;
    }
    return res;
}
