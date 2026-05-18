#pragma once
#include <json/json.h>
#include <string>
#include <vector>

struct IpDoxSuccessInfo {
  std::string continent_code;
  std::string continent;
  bool is_in_european_union;
  std::string country_code;
  std::string country;
  std::vector<std::string> subdivisions;
  std::string city;
  double latitude;
  double longitude;
  int ASN;
  std::string ASO;
};

struct IpDoxInfo {
  std::string result;
  std::optional<IpDoxSuccessInfo> data;
};

struct IpDoxDto {
  std::vector<IpDoxInfo> ips;

  static IpDoxDto fromJson(const Json::Value &j) {
    IpDoxDto ipdoxlist;

    for (const auto &r : j["ips"]) {
      IpDoxInfo ipinfo;

      ipinfo.result = r["result"].asString();
      if (ipinfo.result == "success") {
        IpDoxSuccessInfo ipsuccessinfo;

        ipsuccessinfo.continent_code = r["continent_code"].asString();
        ipsuccessinfo.continent = r["continent"].asString();
        ipsuccessinfo.is_in_european_union = r["is_in_european_union"].asBool();
        ipsuccessinfo.country_code = r["country_code"].asString();
        ipsuccessinfo.country = r["country"].asString();

        for (const auto &subdivision : r["subdivisions"]) {
          ipsuccessinfo.subdivisions.push_back(subdivision.asString());
        }

        ipsuccessinfo.city = r["city"].asString();
        ipsuccessinfo.latitude = r["latitude"].asDouble();
        ipsuccessinfo.longitude = r["longitude"].asDouble();

        ipsuccessinfo.ASN = r["ASN"].asInt();
        ipsuccessinfo.ASO = r["ASO"].asString();

        ipinfo.data = ipsuccessinfo;
      }

      ipdoxlist.ips.push_back(ipinfo);
    }

    return ipdoxlist;
  }
};
