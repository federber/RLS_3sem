#include"coord_conv_test.h"
#include "coordsystem.h"

void show(std::map<size_t, std::vector<Etalon_point>> inp, std::map<size_t, std::vector<GeoData>> res)
{
    std::cout << "INPUT" << std::endl;
    for(auto& el : inp)
    {
        if(el.first !=1) continue;

        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }

    std::cout << "OUTPUT" << std::endl;

    for(auto& el : res)
    {
        if(el.first !=1) continue;

        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }

    std::cout << std::endl << std::endl << std::endl;
}

void test_conv_GeoC_geoD()
{
    std::cout << "test_conv_GeoC_geoD" << std::endl;
    WGS84 wgs1;
    SK42 sk1;
    //PZ90_11 pz1;
    sk1.setGeoD(Point3D(55.8955, 41.2082, 7));
    wgs1 = sk1;
    std::cout << "sk(geoD): " << sk1.getGeoD() << std::endl <<  "wgs(geoD): " << wgs1.getGeoD() << std::endl;
    std::cout << "sk(geoC): " << sk1.getGeoC() << std::endl <<  "wgs(geoC): " << wgs1.getGeoC() << std::endl;
    std::cout << std::endl;
    std::cout << "OK" << std::endl;
}

void test_conv_spherical()
{
    std::cout << "test_conv_spherical" << std::endl;
    std::map<size_t, std::vector<Etalon_point>> inp;
    for(int i = 0; i < 2; ++i )
    {
        for(int j = 0; j < 2; ++j)
        {
            Etalon_point ep;
            ep.trace_id = i;
            //ep.cp_id = j;
            ep.mod_time = i + 100;
            ep.latitude = 200 + j;
            ep.longitude = 300 + j;
            ep.height = 10 + 3 * j;
            ep.coord = {0,0,0};
            M_vec s;
            s.x = 3 + 3 * j;
            s.y = 4 + 4 * j;
            s.z = 5 + 5 * j;
            ep.speed = s;
            ep.accel = {0,0,0};
            inp[i].push_back(ep);
        }
    }

    Coord_converter cc;
    cc.set_rls_coodr(200,300,10);
    cc.set_etalon_traces(inp);
    auto res = cc.get_etalon_traces_spher();
    std::cout << "WGS84, WGS84" << std::endl;
    show(inp, res);
    std::pair<std::string, std::string> ipt = {"PZ90_11", "WGS84"};
    cc.set_inp_par_type(ipt);
    cc.set_rls_coodr(1,1,1);
    cc.set_etalon_traces(inp);
    res = cc.get_etalon_traces_spher();
    std::cout << "PZ90_11, WGS84" << std::endl;
    show(inp, res);
}
