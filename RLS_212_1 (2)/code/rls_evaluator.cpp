#include"rls_evaluator.h"
#include<algorithm>
double interp_coord(double t0, double t1, double x0, double x1, double t)
        { return (x1 - x0) * (t - t0) / (t1 - t0) + x0; }
GeoData binsearch_interp(const std::vector<GeoData>& etalon, const GeoData& pr)
{
    int l = 0;
    int r = etalon.size() - 1;
    if(pr < etalon[l])
    {
        //std::cout << r <<std::endl;
        //std::string str = "binsearch: element ot of range " + (std::to_string(pr.mod_time)) + " [" + std::to_string(etalon[l].mod_time) + "," + std::to_string(etalon[r].mod_time) + "]";

        //throw std::runtime_error(str);
        return etalon[l];
    }
    if(etalon[r] < pr)
        return etalon[r];
    if(pr == etalon[l])
            { return etalon[0]; }
    if(pr == etalon[r])
            { return etalon[0]; }
    while (l <= r) {
        double m = (l + r) / 2;

        // Check if x is present at mid
        if (etalon[(int)m] <= pr and pr <= etalon[(int)m + 1])
        {
            GeoData res;
            res.mod_time = pr.mod_time;
            res.zond_num = pr.zond_num;
            res.cp_id = pr.cp_id;
            res.trace_id = pr.trace_id;
            GeoData p0 = etalon[(int)m];
            GeoData p1 = etalon[(int)m + 1];
            res.azim = interp_coord(p0.mod_time, p1.mod_time, p0.azim, p1.azim, pr.mod_time);
            res.oblique_dist = interp_coord(p0.mod_time, p1.mod_time, p0.oblique_dist, p1.oblique_dist, pr.mod_time);
            res.aim_angle = interp_coord(p0.mod_time, p1.mod_time, p0.aim_angle, p1.aim_angle, pr.mod_time);
            res.rad_speed = interp_coord(p0.mod_time, p1.mod_time, p0.rad_speed, p1.rad_speed, pr.mod_time);

            return res;
        }
         if (etalon[(int)m + 1] < pr)
            l = (int)m + 1;
        else
             r = (int)m;
    }
     throw std::runtime_error("binsearch_interp: couldn't find element");
}


void Evaluator::interpolate(size_t poi_id, size_t etalon_id)
{
    interpolated_tr[poi_id].resize(poi_result[poi_id].size());
    for(int i = 0; i < poi_result[poi_id].size(); ++i)
        interpolated_tr[poi_id][i] = binsearch_interp(traces[etalon_id], poi_result[poi_id][i]);
}
/*
void Evaluator::interpolate()
{
    for(auto& trace : poi_result)
    {
        if(traces.find(trace.first) != traces.end())
            interpolate(trace.first);
    }
}
*/
void Evaluator::setPoiRes(const std::map<size_t, std::vector<GeoData>> & poi_res)
        { poi_result = poi_res; }
void Evaluator::setTraces(const std::map<size_t, std::vector<GeoData>> & tr)
        { traces = tr; }

std::map<size_t, std::vector<GeoData>> Evaluator::getInterpResult() const
        { return interpolated_tr; }

Eval_param Evaluator::getEvalParam(size_t tr_num, const Req_param& par)
{
    //tr_num++;
    eval_dist_accur(tr_num);
    eval_azim_accur(tr_num);
    eval_aim_ang_accur(tr_num);
    eval_rad_sp_accur(tr_num);
    eval_minmax_dist(tr_num, par);
    eval_rh_rlz_coef(tr_num, par);

    return ev_param[tr_num];
}

void Evaluator::eval_dist_accur(size_t tr_num)
{
    double s = 0;
    double s2 = 0;
    double ds = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].oblique_dist - poi_result[tr_num][i].oblique_dist);
        s += a;
        s2 += a * a;
        ds += poi_result[tr_num][i].oblique_dist;
    }
    double fse = std::sqrt(s2 / (interpolated_tr[tr_num].size() - 1));
    double syst = s / interpolated_tr[tr_num].size();
    s=0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].oblique_dist - poi_result[tr_num][i].oblique_dist);
        s += std::pow(a - fse, 2);
    }

    double stdev = std::sqrt(s / (interpolated_tr[tr_num].size() - 1));
    ev_param[tr_num].dist_accur.setCoord(fse,syst,stdev);
}
void Evaluator::eval_azim_accur(size_t tr_num)
{
    double s = 0;
    double s2 = 0;
    double ds = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].azim - poi_result[tr_num][i].azim);
        s += a;
        s2 += a * a;
        ds += std::pow(interpolated_tr[tr_num][i].azim - a, 2);
    }

    double fse = std::sqrt(s2 / (interpolated_tr[tr_num].size() - 1));
    double syst = s / interpolated_tr[tr_num].size();
    s = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].azim - poi_result[tr_num][i].azim);
        s += std::pow(a - fse, 2);
    }
    double stdev = std::sqrt(s / (interpolated_tr[tr_num].size() - 1));
    ev_param[tr_num].azim_accur.setCoord(fse,syst,stdev);
}
void Evaluator::eval_aim_ang_accur(size_t tr_num)
{
    double s = 0;
    double s2 = 0;
    double ds = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].aim_angle - poi_result[tr_num][i].aim_angle);
        s += a;
        s2 += a * a;
        ds += std::pow(interpolated_tr[tr_num][i].aim_angle - a, 2);
    }
    double fse = std::sqrt(s2 / (interpolated_tr[tr_num].size() - 1));
    double syst = s / interpolated_tr[tr_num].size();
    s = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].aim_angle - poi_result[tr_num][i].aim_angle);
        s += std::pow(a - fse, 2);
    }
    double stdev = std::sqrt(s / (interpolated_tr[tr_num].size() - 1));
    ev_param[tr_num].aim_ang_accur.setCoord(fse,syst,stdev);
}
void Evaluator::eval_rad_sp_accur(size_t tr_num)
{
    double s = 0;
    double s2 = 0;
    double ds = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].rad_speed - poi_result[tr_num][i].rad_speed);
        s += a;
        s2 += a * a;
        ds += std::pow(interpolated_tr[tr_num][i].rad_speed - a, 2);
    }
    double fse = std::sqrt(s2 / (interpolated_tr[tr_num].size() - 1));
    double syst = s / interpolated_tr[tr_num].size();
    s = 0;
    for(int i = 0;i < interpolated_tr[tr_num].size();++i)
    {
        double a = std::abs(interpolated_tr[tr_num][i].rad_speed - poi_result[tr_num][i].rad_speed);
        s += std::pow(a - fse, 2);
    }
    double stdev = std::sqrt(s / (interpolated_tr[tr_num].size() - 1));
    ev_param[tr_num].rad_sp_accur.setCoord(fse,syst,stdev);
}
void Evaluator::eval_rh_rlz_coef(size_t tr_num, const Req_param& par)
{
    auto &el = ev_param[tr_num];
    std::cout << el.max_dist.first << "   " << (el.max_dist.second) <<" "<< par.h_c << std::endl;
    ev_param[tr_num].rh_rlz_coef = el.max_dist.first / 1000 / 4.12 / (std::sqrt(el.max_dist.second) + std::sqrt(par.h_c ));
}
void Evaluator::eval_minmax_dist(size_t tr_num, const Req_param& par)
{
    std::vector<std::pair<double,double>> vis_coef;
    auto a_trace = interpolated_tr[tr_num];

    double v_pr = 1; // скорость на предыдущем промежутке
    double s_v = 0;
    for(int i = 1; i < a_trace.size(); ++i)
    {
        auto& ap = a_trace[i];
        double v = (ap.oblique_dist - a_trace[i-1].oblique_dist) / (ap.mod_time - a_trace[i-1].mod_time);
        s_v += v;
        if(i != 1 and v_pr * v < 0)
            throw std::runtime_error("wrong_input: distant function should grow monotonous");
        v_pr = v;
    }
    double v_apr = s_v / (a_trace.size()-1);

    bool is_grow;
    if(v_apr > 0)
        is_grow = true;
    else is_grow = false;

    v_apr = (v_apr);

    if(!is_grow)
    {
        reverse(a_trace.begin(), a_trace.end());
    }
    std::pair<double,double> d_min = {a_trace[0].oblique_dist,a_trace[0].mod_time};
    std::pair<double,double> d_max = {a_trace[a_trace.size()-1].oblique_dist,a_trace[a_trace.size()-1].mod_time};
    double dD = par.N_obz * par.T_obz * std::abs(v_apr);
    double D = d_min.first;
    std::cout << "v_apr: " << v_apr << "d_min_0: " << d_min.first << " t_min: " << d_min.second << " d_max_0: " << d_max.first << " t_max: " << d_max.second << " dD: " << dD << std::endl;
    for(auto& el : a_trace){
        std::cout << el.mod_time << "  " << el.oblique_dist << std::endl;
    }
    double t_0 = a_trace[0].mod_time;
    while(D <= d_max.first)
    {
        auto N_obn = num_of_detect_on_segm(a_trace, D, D+dD);
        std::cout << "coef: " << (double)N_obn / par.N_obz << " dist: " << D+dD/2 <<" time: " << t_0 + (dD/2)/v_apr << std::endl;
        vis_coef.push_back({(double)N_obn / par.N_obz, t_0 + (dD/2)/v_apr});
        D += dD / 2;
        t_0 += (dD/2)/v_apr;
    }

    bool ip = false;
    bool m = false;
    double d_pr_m = std::abs(v_apr) * par.L_sb * par.T_obz;
    std::pair<double,double> s_pr = {d_max.first + 2*d_pr_m, -1};
    std::pair<double,double> e_pr = {s_pr.first + 2*d_pr_m, -2};
    double d_min_0 = d_min.first;
   // double t_min = vis_coef[0].second;
   // double t_max = vis_coef[vis_coef.size()].second;
    for(int i = 0; i < vis_coef.size(); ++i){
        if(vis_coef[i].first >= 0.5)
        {
            if(!ip and m)
            {
                continue;
            }
            if(!ip and !m)
            {
                d_min.first = d_min_0 + dD/2*(i+1);
                d_min.second = vis_coef[i].second;
               // std::cout << "d_min: " << d_min.first << " t_min: " << d_min.second << std::endl;

                m = true;
                continue;
            }
            if(ip and !m)
            {
                if(e_pr.first > d_min_0 + dD/2*(i+1)){
                    e_pr.first = d_min_0 + dD/2*(i+1);
                    e_pr.second = vis_coef[i].second;
                }
                //e_pr = std::min(e_pr, d_min_0 + dD/2*(i+1));
                m = true;
                if(e_pr .first - s_pr.first <= d_pr_m)
                {
                    continue;
                }
                else
                {
                    d_max = s_pr;
                    std::cout  << " d_max: " << d_max.first << " t_max: " << d_max.second << std::endl;

                    //d_max.second = vis_coef[i].second;
                    break;
                }
            }
            if(ip and m)
            {
                continue;
            }

        }
        else
        {
           if(!ip and !m)
           {
               continue;
           }
           if(!ip and m)
           {
               if(s_pr.first > d_min_0 + dD/2*(i+1)){
                   s_pr.first = d_min_0 + dD/2*(i+1);
                   s_pr.second = vis_coef[i].second;

               }
               //s_pr = std::min(s_pr, d_min_0 + dD/2*(i+1));

               m = false;
               ip = true;
               continue;
           }
           if(ip and !m)
           {
               continue;
           }
           if(ip and m)
           {

               d_max.first = d_min_0 + dD/2*(i+1);
               d_max.second = vis_coef[i].second;
               std::cout << " d_max: " << d_max.first << " t_max: " << d_max.second << std::endl;

               break;
           }
        }
    }
    if(ip and !m and vis_coef[vis_coef.size()-1].first < 0.5)
    {
        d_max.first = s_pr.first - dD/2; // последний промежуток где был K > 0.5
        d_max.second = s_pr.second - dD/2/v_apr;
    }
    std::cout << "d_min: " << d_min.first << " t_min: " << d_min.second << " d_max: " << d_max.first << " t_max: " << d_max.second << std::endl;
    std::pair<std::pair<double,double>,std::pair<double,double>> res;
    res.first.first = d_min.first;
    res.second.first = d_max.first;
    GeoData m_dist{d_min.second, 1, 1, tr_num, 1, 0, 1, 0};
    GeoData rfh = binsearch_interp(interpolated_tr[tr_num], m_dist);
    std::cout << "OD: " << rfh.oblique_dist << "  SIN: " << (rfh.aim_angle) << std::endl;

    //std::cout << rfh.oblique_dist << "   " << d_min.first << std::endl;
    res.first.second = rfh.oblique_dist * std::sin(rfh.aim_angle);
    m_dist.mod_time = d_max.second;
    rfh = binsearch_interp(interpolated_tr[tr_num], m_dist);
    res.second.second = rfh.oblique_dist * std::sin(rfh.aim_angle);
    std::cout << "MIN:" << std::endl;
    std::cout << "min_dist: " << res.first.first << " min_height: " << res.first.second << std::endl;
    std::cout << "MAX:" << std::endl;
    std::cout << "max_dist: " << res.second.first << " max_height: " << res.second.second << std::endl;
    ev_param[tr_num].min_dist = res.first;
    ev_param[tr_num].max_dist = res.second;


}
size_t num_of_detect_on_segm(const std::vector<GeoData>& traces, double a, double b)
{

        auto bs = [&](double p){
            int l = 0;
            int r = traces.size() - 1;

            while (l <= r) {

                double m = (l + r) / 2;

                if (traces[(int)m].oblique_dist <= p and p <= traces[(int)m + 1].oblique_dist)
                    return (int)m;
                if (traces[(int)m + 1].oblique_dist < p)
                    l = (int)m + 1;
                else
                    r = (int)m;
            }
        };
        auto bsa = bs(a);
        auto bsb = bs(b);
        if(a <= traces[0].oblique_dist)
            bsa = -1;
        if(traces[traces.size()-1].oblique_dist <= b)
            bsb = traces.size()-1;
        if(a > traces[traces.size()-1].oblique_dist or b < traces[0].oblique_dist)
                return 0;
        return bsb - bsa;


}




std::ostream& operator<<(std::ostream &out, Eval_param& ep)
{
    out << "EVAL_PARAM: " << std::endl;
    out << "dist_accur: " << ep.dist_accur << std::endl;
    out << "azim_accur: " << ep.azim_accur << std::endl;
    out << "aim_ang_accur: " << ep.aim_ang_accur << std::endl;
    out << "rad_sp_accur: " << ep.rad_sp_accur << std::endl;
    out << "min_dist: " << ep.min_dist.first << " in H = " << ep.min_dist.second << std::endl;
    out << "max_dist: " << ep.max_dist.first << " in H = " << ep.max_dist.second << std::endl;
    out << "rh_c: " << ep.rh_rlz_coef << std::endl;
    return out;
}

