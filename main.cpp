#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

class CIterator;

class CLandRegister
{
public:
    struct Property{
        std::string m_City;
        std::string m_Addr;
        std::string m_Region;
        unsigned long long m_ID;
        std::string m_Owner;
        long long m_AcquisitionTimestamp;
    };

    CLandRegister();
    ~CLandRegister();

    bool add(const std::string& city, const std::string& addr,
             const std::string& region, unsigned long long id);

    bool del(const std::string& city, const std::string& addr);

    bool del(const std::string    & region, unsigned long long id);

    bool getOwner(const std::string& city, const std::string& addr,
                  std::string& owner) const;

    bool getOwner(const std::string& region, unsigned long long id,
                  std::string& owner) const;

    bool newOwner(const std::string& city, const std::string& addr,
                  const std::string& owner);

    bool newOwner(const std::string& region, unsigned long long id,
                  const std::string& owner);

    size_t count(const std::string& owner) const;

    CIterator listByAddr() const;

    CIterator listByOwner(const std::string& owner) const;

    size_t findProperty(const std::string& city, const std::string& addr) const;

    size_t findProperty(const std::string& region, unsigned long long id) const;
private:
    friend class CIterator;
    std::vector<Property> sortedByCityAddr;
    std::vector<Property> sortedByRegionID;
    size_t m_NextAcquisitionOrder = 1;
};

class CIterator
{
public:
    CIterator(const CLandRegister &landRegister, std::vector<CLandRegister::Property> sortedProperties);
    ~CIterator();

    bool atEnd() const;
    void next();
    std::string city() const;
    std::string addr() const;
    std::string region() const;
    unsigned id() const;
    std::string owner() const;
private:
    friend class CLandRegister;
    const CLandRegister &landRegister;
    size_t currentIndex;
    std::vector<CLandRegister::Property> sortedProperties;
};

CLandRegister::CLandRegister() {}

CLandRegister::~CLandRegister() {}

CIterator::CIterator(const CLandRegister& landRegister, std::vector<CLandRegister::Property> sortedProperties)
        : landRegister(landRegister), currentIndex(0), sortedProperties(sortedProperties) {}

CIterator::~CIterator() {}

bool CLandRegister::add(const std::string& city, const std::string& addr, const std::string& region, unsigned long long id) {
    Property targetProperty;
    targetProperty.m_City = city;
    targetProperty.m_Addr = addr;
    targetProperty.m_Region = region;
    targetProperty.m_ID = id;

    // Check if the property already exists
    if (std::binary_search(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                           [&](const Property& a, const Property& b){
                               return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                           }) ||
        std::binary_search(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                           [&](const Property& a, const Property& b) {
                               return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                           })) {
        return false; // Property already exists
    }

    // Insert the property into both lists
    auto it1 = std::lower_bound(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                                [&](const Property& a, const Property& b){
                                    return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                                });
    sortedByCityAddr.insert(it1, targetProperty);

    auto it2 = std::lower_bound(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                                [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                                });
    sortedByRegionID.insert(it2, targetProperty);

    return true;
}

bool CLandRegister::del(const std::string& city, const std::string& addr)
{
    Property targetProperty;
    targetProperty.m_City = city;
    targetProperty.m_Addr = addr;

    if (!std::binary_search(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                            [&](const Property& a, const Property& b){
                                return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                            }) &&
        !std::binary_search(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                            [&](const Property& a, const Property& b) {
                                return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                            })) {
        return false; // Property already exists
    }

    auto it1 = std::lower_bound(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                                [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                                });
    auto it2 = std::lower_bound(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                                [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                                });

    sortedByRegionID.erase(it1);
    sortedByCityAddr.erase(it2);
    return true;
}

bool CLandRegister::del(const std::string &region, unsigned long long id)
{
    Property targetProperty;
    targetProperty.m_Region = region;
    targetProperty.m_ID = id;

    if (!std::binary_search(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                           [&](const Property& a, const Property& b){
                               return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                           }) &&
        !std::binary_search(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                           [&](const Property& a, const Property& b) {
                               return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                           })) {
        return false; // Property already exists
    }

    auto it1 = std::lower_bound(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                               [&](const Property& a, const Property& b) {
                                   return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                               });
    auto it2 = std::lower_bound(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                               [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                                });

    sortedByRegionID.erase(it1);
    sortedByCityAddr.erase(it2);
    return true;
}

bool CLandRegister::getOwner(const std::string& city, const std::string& addr, std::string& owner) const
{
    Property targetProperty;
    targetProperty.m_City = city;
    targetProperty.m_Addr = addr;

    if (!std::binary_search(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                            [&](const Property& a, const Property& b){
                                return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                            }) &&
        !std::binary_search(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                            [&](const Property& a, const Property& b) {
                                return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                            })) {
        return false; // Property already exists
    }

    auto it1 = std::lower_bound(sortedByRegionID.begin(), sortedByRegionID.end(), targetProperty,
                                [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_Region, a.m_ID) < std::tie(b.m_Region, b.m_ID);
                                });
    auto it2 = std::lower_bound(sortedByCityAddr.begin(), sortedByCityAddr.end(), targetProperty,
                                [&](const Property& a, const Property& b) {
                                    return std::tie(a.m_City, a.m_Addr) < std::tie(b.m_City, b.m_Addr);
                                });

    auto& itToGetOwner1 = it1;
    auto& itToGetOwner2 = it2;
    owner = itToGetOwner1->m_Owner;
    owner = itToGetOwner2->m_Owner;
    return true;
}

bool CLandRegister::getOwner(const std::string& region, unsigned long long id, std::string& owner) const
{
    size_t index = findProperty(region, id);
    if (index == properties.size()) {
        return false; // Property not found
    }

    owner = properties[index].m_Owner ;
    return true;
}

bool CLandRegister::newOwner(const std::string& city, const std::string& addr, const std::string& owner)
{
    size_t index = findProperty(city, addr);
    if (index == properties.size() || properties[index].m_Owner == owner) {
        return false; // Property not found
    }

    properties[index].m_Owner = owner;
    properties[index].m_AcquisitionTimestamp = m_NextAcquisitionOrder++;

    return true;
}

bool CLandRegister::newOwner(const std::string& region, unsigned long long id, const std::string& owner)
{
    size_t index = findProperty(region, id);
    if (index == properties.size() || properties[index].m_Owner == owner) {
        return false; // Property not found
    }

    properties[index].m_Owner = owner;
    properties[index].m_AcquisitionTimestamp = m_NextAcquisitionOrder++;

    return true;
}

size_t CLandRegister::count(const std::string& owner) const
{
    return std::count_if(sortedByCityAddr.begin(), sortedByCityAddr.end(),
                         [owner](const Property& p) {
                             return strcasecmp(p.m_Owner.c_str(), owner.c_str()) == 0;
                         });
}


CIterator CLandRegister::listByAddr() const
{
    CIterator iterator(*this, sortedByCityAddr);
    return iterator;
}

bool CIterator::atEnd() const
{
    return currentIndex >= sortedProperties.size();
}

void CIterator::next()
{
    if(!atEnd())
    {
        currentIndex++;
    }
}

std::string CIterator::city() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_City : "";
}

std::string CIterator::addr() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Addr : "";
}

std::string CIterator::owner() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Owner : "";
}

std::string CIterator::region() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_Region : "";
}

unsigned CIterator::id() const
{
    return (!atEnd()) ? sortedProperties[currentIndex].m_ID : 0;
}

static void test0 () {
    CLandRegister x;
    std::string owner;

    assert (x.add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.add("Prague", "Technicka", "Dejvice", 9873));
    assert (x.add("Plzen", "Evropska", "Plzen mesto", 78901));
    assert (x.add("Liberec", "Evropska", "Librec", 4552));

    CIterator i0 = x.listByAddr();
    assert (!i0.atEnd()
            && i0.city() == "Liberec"
            && i0.addr() == "Evropska"
            && i0.region() == "Librec"
            && i0.id() == 4552
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Plzen"
            && i0.addr() == "Evropska"
            && i0.region() == "Plzen mesto"
            && i0.id() == 78901
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Evropska"
            && i0.region() == "Vokovice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Technicka"
            && i0.region() == "Dejvice"
            && i0.id() == 9873
            && i0.owner() == "");
    i0.next();
    assert (!i0.atEnd()
            && i0.city() == "Prague"
            && i0.addr() == "Thakurova"
            && i0.region() == "Dejvice"
            && i0.id() == 12345
            && i0.owner() == "");
    i0.next();
    assert (i0.atEnd());

    assert (x.count("") == 5);

    assert (x.del("Prague", "Thakurova"));
    assert (x.del("Prague", "Evropska"));
    assert (x.del("Prague", "Technicka"));
    assert (x.del("Plzen", "Evropska"));
    assert (x.del("Liberec", "Evropska"));

    assert (x.count("") == 0);
}

int main ( void )
{
    test0 ();
    return EXIT_SUCCESS;
}