#pragma once

#include <xhash>

class UUID
{
public:
   UUID();
   UUID(uint64_t uuid);
   UUID(const UUID& uuid);

   operator uint64_t() const { return m_UUID; }
private:
   uint64_t m_UUID;
};

// Hash UUID
namespace std
{
   template<>
   struct hash<UUID>
   {
      std::size_t operator()(const UUID& uuid) const
      {
         return hash<uint64_t>()((uint64_t)uuid);
      }
   };
}