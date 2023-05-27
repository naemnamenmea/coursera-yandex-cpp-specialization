#include "geo2d.h"
#include "game_object.h"

#include "test_runner.h"

#include <vector>
#include <memory>

using namespace std;

// Определите классы Unit, Building, Tower и Fence так, чтобы они наследовались от
// GameObject и реализовывали его интерфейс.

template<typename T>
struct Collider : public GameObject
{
  bool Collide(const GameObject& that) const final
  {
    return that.CollideWith(static_cast<const T&>(*this));
  }
};

class Unit final : public Collider<Unit> {
public:
  explicit Unit(const geo2d::Point& position)
    : m_position(position) {}

  const geo2d::Point& GetPosition() const
  {
    return m_position;
  }

  bool CollideWith(const Unit& that) const override;
  bool CollideWith(const Fence& that) const override;
  bool CollideWith(const Building& that) const override;
  bool CollideWith(const Tower& that) const override;

private:
  geo2d::Point m_position;
};

class Building final : public Collider<Building> {
public:
  explicit Building(const geo2d::Rectangle& position)
    :m_position(position) {}

  const geo2d::Rectangle& GetPosition() const
  {
    return m_position;
  }

  bool CollideWith(const Unit& that) const override;
  bool CollideWith(const Fence& that) const override;
  bool CollideWith(const Building& that) const override;
  bool CollideWith(const Tower& that) const override;

private:
  geo2d::Rectangle m_position;
};

class Tower final : public Collider<Tower> {
public:
  explicit Tower(const geo2d::Circle& position)
    : m_position(position) {}

  const geo2d::Circle& GetPosition() const
  {
    return m_position;
  }

  bool CollideWith(const Unit& that) const override;
  bool CollideWith(const Fence& that) const override;
  bool CollideWith(const Building& that) const override;
  bool CollideWith(const Tower& that) const override;

private:
  geo2d::Circle m_position;
};

class Fence final : public Collider<Fence> {
public:
  explicit Fence(const geo2d::Segment& position)
    : m_position(position) {}

  const geo2d::Segment& GetPosition() const
  {
    return m_position;
  }

  bool CollideWith(const Unit& that) const override;
  bool CollideWith(const Fence& that) const override;
  bool CollideWith(const Building& that) const override;
  bool CollideWith(const Tower& that) const override;

private:
  geo2d::Segment m_position;
};

#define EXPAND(TypeOne, TypeTwo) \
bool TypeOne::CollideWith(const TypeTwo& that) const \
{ \
  return geo2d::Collide(m_position, that.GetPosition()); \
}

//#define GET_ITEM(item) item
//#define APPLY_1(m, a) m(a)
//#define APPLY_2(m, a, b) m(a, b)
//
//#define EXEC_2(m, item, head) APPLY_2(m, item, head);
//#define EXEC_3(m, item, head, ...) APPLY_2(m, item, head); EXEC_2(m, item, __VA_ARGS__)
//#define EXEC_4(m, item, head, ...) APPLY_2(m, item, head); EXEC_3(m, item, __VA_ARGS__)
//#define EXEC_5(m, item, head, ...) APPLY_2(m, item, head); EXEC_4(m, item, __VA_ARGS__)
//
//#define UNROLL_1(m, list1, item) EXEC_2(m, item, list1);
//#define UNROLL_2(m, list1, item, ...) EXEC_3(m, item, list1); UNROLL_1(m, list1, __VA_ARGS__)
//#define UNROLL_3(m, list1, item, ...) EXEC_4(m, item, list1); UNROLL_2(m, list1, __VA_ARGS__)
//#define UNROLL_4(m, list1, item, ...) EXEC_5(m, item, list1); UNROLL_3(m, list1, __VA_ARGS__)
//
//#define RUN_4(m, ...) UNROLL_4(m, (__VA_ARGS__), __VA_ARGS__)
//RUN_4(EXPAND, Fence, Tower, Unit, Building);

EXPAND(Unit, Fence);
EXPAND(Unit, Tower);
EXPAND(Unit, Unit);
EXPAND(Unit, Building);

EXPAND(Fence, Fence);
EXPAND(Fence, Tower);
EXPAND(Fence, Unit);
EXPAND(Fence, Building);

EXPAND(Tower, Fence);
EXPAND(Tower, Tower);
EXPAND(Tower, Unit);
EXPAND(Tower, Building);

EXPAND(Building, Fence);
EXPAND(Building, Tower);
EXPAND(Building, Unit);
EXPAND(Building, Building);

// Реализуйте функцию Collide из файла GameObject.h

bool Collide(const GameObject& first, const GameObject& second) {
  return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
  // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то объекты,
  // и мы хотим добавить на неё новый, например, построить новое здание или башню.
  // Мы можем его добавить, только если он не пересекается ни с одним из существующих.
  using namespace geo2d;

  const vector<shared_ptr<GameObject>> game_map = {
    make_shared<Unit>(Point{3, 3}),
    make_shared<Unit>(Point{5, 5}),
    make_shared<Unit>(Point{3, 7}),
    make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
    make_shared<Tower>(Circle{Point{9, 4}, 1}),
    make_shared<Tower>(Circle{Point{10, 7}, 1}),
    make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
  };

  for (size_t i = 0; i < game_map.size(); ++i) {
    Assert(
      Collide(*game_map[i], *game_map[i]),
      "An object doesn't collide with itself: " + to_string(i)
    );

    for (size_t j = 0; j < i; ++j) {
      Assert(
        !Collide(*game_map[i], *game_map[j]),
        "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
      );
    }
  }

  auto new_warehouse = make_shared<Building>(Rectangle{ {4, 3}, {9, 6} });
  ASSERT(!Collide(*new_warehouse, *game_map[0]));
  ASSERT(Collide(*new_warehouse, *game_map[1]));
  ASSERT(!Collide(*new_warehouse, *game_map[2]));
  ASSERT(Collide(*new_warehouse, *game_map[3]));
  ASSERT(Collide(*new_warehouse, *game_map[4]));
  ASSERT(!Collide(*new_warehouse, *game_map[5]));
  ASSERT(!Collide(*new_warehouse, *game_map[6]));

  auto new_defense_tower = make_shared<Tower>(Circle{ {8, 2}, 2 });
  ASSERT(!Collide(*new_defense_tower, *game_map[0]));
  ASSERT(!Collide(*new_defense_tower, *game_map[1]));
  ASSERT(!Collide(*new_defense_tower, *game_map[2]));
  ASSERT(Collide(*new_defense_tower, *game_map[3]));
  ASSERT(Collide(*new_defense_tower, *game_map[4]));
  ASSERT(!Collide(*new_defense_tower, *game_map[5]));
  ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestAddingNewObjectOnMap);
  return 0;
}
