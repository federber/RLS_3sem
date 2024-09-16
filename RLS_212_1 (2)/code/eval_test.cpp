#include"eval_test.h"
#include"reading_parser.h"
#include"coord_conv_test.h"
void test_evaluator()
{
    std::cout << "test_evaluator" << std::endl;
    Evaluator eval;
    std::map<size_t, std::vector<GeoData>> poi_res;
    std::map<size_t, std::vector<GeoData>> etalon;
    size_t tr_id = 10;
    for(int i = 1; i < 6; ++i)
    {
        GeoData p1;
        p1.mod_time = 10 * i;
        p1.aim_angle = (double)(10 + 10 * i)/180*PI;
        p1.azim = (double)(10 + 10 * i)/180*PI;
        p1.oblique_dist = 1000 * i;
        p1.trace_id = tr_id;
        p1.cp_id = i;
        etalon[tr_id].push_back(p1);
    }

    for(int i = 1; i < 5; ++i)
    {
        GeoData p1;
        p1.mod_time = 10 * i + 5;
        p1.aim_angle = (double)(10 * i + 5)/180*PI;
        p1.azim = (double)(10 * i + 5)/180*PI;
        p1.oblique_dist = 1000 * i + 500;
        p1.trace_id = tr_id;
        p1.cp_id = i;
        poi_res[tr_id].push_back(p1);
    }

    eval.setPoiRes(poi_res);
    eval.setTraces(etalon);


    std::cout << "INPUT" << std::endl;
    std::cout << "POI_RES" << std::endl;
    for(auto& el : poi_res)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }
    std::cout << "ETALON" << std::endl;
    for(auto& el : etalon)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }

    eval.interpolate(1,1);
    auto res = eval.getInterpResult();

    std::cout << "OUTPUT" << std::endl;

    for(auto& el : res)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }
    Req_param par;
    par.T_obz = 5;
    par.h_c = 10;
    par.L_sb = 3;
    par.N_obz = 3;
    eval.getEvalParam(10,par);

}


void test_evaluator_decr()
{
    std::cout << "test_evaluator" << std::endl;
    Evaluator eval;
    std::map<size_t, std::vector<GeoData>> poi_res;
    std::map<size_t, std::vector<GeoData>> etalon;
    size_t tr_id = 10;
    for(int i = 1; i < 6; ++i)
    {
        GeoData p1;
        p1.mod_time = (10 * i);
        p1.aim_angle = double(100-(10 + 10 * i))/180*PI;
        p1.azim = double(100-(10 + 10 * i))/180*PI;
        p1.oblique_dist = 6000-(1000 * i);
        p1.trace_id = tr_id;
        p1.cp_id = i;
        etalon[tr_id].push_back(p1);
    }

    for(int i = 1; i < 5; ++i)
    {
        GeoData p1;
        p1.mod_time = (10 * i + 5);
        p1.aim_angle = double(100-(10 * i + 5))/180*PI;
        p1.azim = double(100-(10 * i + 5))/180*PI;
        p1.oblique_dist = 6000-(1000 * i + 500);
        p1.trace_id = tr_id;
        p1.cp_id = i;
        poi_res[tr_id].push_back(p1);
    }

    eval.setPoiRes(poi_res);
    eval.setTraces(etalon);


    std::cout << "INPUT" << std::endl;
    std::cout << "POI_RES" << std::endl;
    for(auto& el : poi_res)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }
    std::cout << "ETALON" << std::endl;
    for(auto& el : etalon)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }

    eval.interpolate(1,1);
    auto res = eval.getInterpResult();

    std::cout << "OUTPUT" << std::endl;

    for(auto& el : res)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }
    Req_param par;
    par.T_obz = 5;
    par.h_c = 10;
    par.L_sb = 3;
    par.N_obz = 3;
    eval.getEvalParam(10,par);

}


void test_with_file_input()
{
    std::cout << "TEST WITH FILE INPUT" << std::endl;
    Coord_converter conv;
    conv.set_rls_coodr(55.89558/180*PI, 41.206408/180*PI, 177);
    auto inp = parser_etalon(readFileData("etalon_3.txt"));
    std::cout << "input map size: " << inp.size() << std::endl;
    conv.set_etalon_traces(inp);
    auto etl = conv.get_etalon_traces_spher();

    //show(inp, etl);
    auto poi_res = parser_geodata(readFileData("poi_out_3.txt"));
    /*
    std::cout << "POI_RES" << std::endl;
    for(auto& el : poi_res)
    {
        std::cout << "trace_id: " << el.first << std::endl;
        for(auto& point : el.second)
        {
            std::cout << point << std::endl;
        }
    }
    */
    Evaluator ev;
    ev.setPoiRes(poi_res);
    ev.setTraces(etl);
    size_t poi_n = 2;
    size_t etl_n = 1;
    ev.interpolate(poi_n,etl_n);
    auto ipr = ev.getInterpResult();
    std::cout << "ETALON" << std::endl;
    for(auto& el : etl[etl_n]){
        std::cout << el << std::endl;
    }
    std::cout << std::endl << std::endl;

    std::cout << "POI_RES" << std::endl;
    for(auto& el : poi_res[poi_n]){
        std::cout << el << std::endl;
    }
    std::cout << std::endl << std::endl;

    std::cout << "INTERPOLATED" << std::endl;

    for(auto& el : ipr[poi_n]){
        std::cout << el << std::endl;
    }



    Req_param par;
    par.T_obz = 10;
    par.h_c = 10;
    par.L_sb = 5;
    par.N_obz = 7;
    auto res = ev.getEvalParam(poi_n,par);
    std::cout << res;
}
