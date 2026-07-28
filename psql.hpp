#include <libpq-fe.h>

namespace Koral {
using PGcn_t = PGconn*;
using PGrs_t = PGresult*;
namespace PG {
  PGcn_t login(const std::string& var) {
    return PQconnectdb(var.c_str());
  }
  
  PGcn_t login(std::string_view var) {
    return PQconnectdb(var.data());
  }
  
  void end(PGcn_t var) { if (var) PQfinish(var); }
  
  PGcn_t login(const char* var) {
    return PQconnectdb(var);
  }
  
  template <typename Func>
  void OnFail(PGcn_t var, Func func) {
    if (!var || PQstatus(var) != CONNECTION_OK) {
      func();
    }
  }
  
  template <typename Func>
  void OnSucc(PGcn_t var, Func func) {
    if (var && PQstatus(var) == CONNECTION_OK) {
      func();
    }
  }

  auto RsErrMsg(PGrs_t var) {
    return PQresultErrorMessage(var);
  }
  
  auto ErrMsg(PGcn_t var) {
    return PQerrorMessage(var);
  }
  
  template <typename Func>
  void OnFail(PGrs_t var, Func func) {
    if (!var) {
      func();
      return;
    }
    auto status = PQresultStatus(var);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
      func();
    }
  }
  
  template <typename Func>
  void OnSucc(PGrs_t var, Func func) {
    if (var && (PQresultStatus(var) == PGRES_TUPLES_OK || PQresultStatus(var) == PGRES_COMMAND_OK)) {
      func();
    }
  }
  
  template <typename T>
  auto GetVal(PGrs_t& var0, T val0, T val1) { return PQgetvalue(var0, val0, val1); }
  
  auto clear(PGrs_t var) {
    return PQclear(var);
  }
  
  PGrs_t exec(PGcn_t conn, const char* sql) {
      return PQexec(conn, sql);
  }
  
  PGrs_t exec(PGcn_t conn, const std::string& sql) {
      return PQexec(conn, sql.c_str());
  }
  
  PGrs_t exec(PGcn_t conn, std::string_view sql) {
      return PQexec(conn, sql.data());
  }
}
}
