#include <ostream>
#include <fstream>
#include "parameter.h"
#include "cmd_line.h"



namespace cine2 {


#define clp_required(x) (param.x = clp.required<decltype(param.x )>( #x ))
#define clp_optional_val(x, val) (param.x = clp.optional_val( #x, val))
#define clp_optional_vec(x, val) (clp.optional_vec( #x, val))


  Param parse_parameter(cmd::cmd_line_parser& clp)
  {

    Param param;
    clp_required(G);
    clp_required(t_scenes);

    clp_required(pop_size);
    clp_required(dims);
    clp_required(resource_min);
    clp_required(resource_max);

    clp_optional_val(mutation_rate, 0.01);
    clp_optional_val(mutation_shape, 0.01);
    clp_optional_val(cost, 0.0001);
    clp_optional_val(cost_comp, 0.0001);
    clp_optional_val(changerate, 0.05);
    clp_optional_val(changeprop, 0.05);
    clp_optional_val(seed, 1111);

    clp_optional_val(outdir, std::string{});



    return param;
  }

#undef clp_required
#undef clp_optional_val
#undef clp_optional_vec


  cmd::cmd_line_parser config_file_parser(const std::string& config)
  {
    std::ifstream is(config);
    if (!is) throw cmd::parse_error("can't open config file");
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    return cmd::cmd_line_parser(str);
  }


#define stream(x) (os << prefix << #x "=" << param.x << postfix)
#define stream_str(x) (os << prefix << #x "=\"" << param.x << '"' << postfix)
#define stream_array(x) (do_stream_array(os << prefix, #x, param.x, lb, rb) << postfix)


  namespace {

    template <typename C>
    std::ostream& do_stream_array(std::ostream& os, const char* name, const C& cont, const char* lb, const char* rb)
    {
      os << name << '=' << lb;
      for (size_t i = 0; i < cont.size() - 1; ++i) {
        os << cont[i] << ',';
      }
      os << cont.back() << rb;
      return os;
    }

  }


  std::ostream& stream_parameter(std::ostream& os,
    const Param& param,
    const char* prefix,
    const char* postfix,
    const char* lb = "{",
    const char* rb = "}")
  {
    stream(G);
    stream(t_scenes);

    stream(pop_size);
    stream(dims);
    stream(resource_min);
    stream(resource_max);

    stream(mutation_rate);
    stream(mutation_shape);
    stream(cost);
    stream(cost_comp);
    stream(changeprop);
    stream(seed);


    stream_str(outdir);
    os << '\n';


    return os;
  }

#undef stream
#undef stream_str
#undef stream_array

}