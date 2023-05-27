#include "Common.h"
#include <stdexcept>

using namespace std;

// Этот файл сдаётся на проверку
// Здесь напишите реализацию необходимых классов-потомков `IShape`

class ShapeBase : public IShape
{
public:
  ShapeBase() :m_size{ 0,0 }, m_position{ 0,0 } {}

  void SetPosition(Point newPosition) override
  {
    m_position = move(newPosition);
  }

  Point GetPosition() const override
  {
    return m_position;
  }

  void SetSize(Size newSize) override
  {
    m_size = newSize;
  }

  Size GetSize() const override
  {
    return m_size;
  }

  void SetTexture(std::shared_ptr<ITexture> newTexture) override
  {
    m_texture = move(newTexture);
  }

  ITexture* GetTexture() const override
  {
    return m_texture.get();
  }

protected:
  shared_ptr<ITexture> m_texture;
  Point m_position;
  Size m_size;
};

class Rectangle : public ShapeBase
{
public:
  std::unique_ptr<IShape> Clone() const override
  {
    std::unique_ptr<IShape> copy = make_unique<Rectangle>();
    copy->SetSize(m_size);
    copy->SetPosition(m_position);
    copy->SetTexture(m_texture);

    return copy;
  }

  void Draw(Image& image) const override
  {
    const Size& textureSize = m_texture ? m_texture->GetSize() : Size{ 0, 0 };
    const Image& textureImage = m_texture ? m_texture->GetImage() : Image{};

    const size_t rowLimit = min(static_cast<size_t>(m_position.y + m_size.height), image.size());
    const size_t colLimit = min(static_cast<size_t>(m_position.x + m_size.width), image.empty() ? 0 : image[0].size());

    for (size_t i = m_position.y; i < rowLimit; ++i)
    {
      for (size_t j = m_position.x; j < colLimit; ++j)
      {
        size_t textureX = j - m_position.x;
        size_t textureY = i - m_position.y;

        const bool outOfTexture = !(textureX < textureSize.width && textureY < textureSize.height);

        image[i][j] = outOfTexture ? '.' : textureImage[textureY][textureX];
      }
    }
  }
};

class Ellipse : public ShapeBase
{
public:
  std::unique_ptr<IShape> Clone() const override
  {
    std::unique_ptr<IShape> copy = make_unique<Ellipse>();
    copy->SetSize(m_size);
    copy->SetPosition(m_position);
    copy->SetTexture(m_texture);

    return copy;
  }

  void Draw(Image& image) const override
  {
    const Size& textureSize = m_texture ? m_texture->GetSize() : Size{ 0, 0 };
    const Image& textureImage = m_texture ? m_texture->GetImage() : Image{};

    const size_t rowLimit = min(static_cast<size_t>(m_position.y + m_size.height), image.size());
    const size_t colLimit = min(static_cast<size_t>(m_position.x + m_size.width), image.empty() ? 0 : image[0].size());

    for (size_t i = m_position.y; i < rowLimit; ++i)
    {
      for (size_t j = m_position.x; j < colLimit; ++j)
      {
        size_t textureX = j - m_position.x;
        size_t textureY = i - m_position.y;

        const Point pnt = {(int)textureX, m_size.height - (int)textureY - 1};
        const bool outOfShape = !IsPointInEllipse(pnt, m_size);

        if (!outOfShape)
        {
          const bool outOfTexture = !(textureX < textureSize.width && textureY < textureSize.height);
          image[i][j] = outOfTexture ? '.' : textureImage[textureY][textureX];
        }
      }
    }
  }
};

// Напишите реализацию функции
unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type)
  {
  case ShapeType::Ellipse:
    return make_unique<Ellipse>();
  case ShapeType::Rectangle:
    return make_unique<Rectangle>();
  default: throw runtime_error("Unsupported shape type");
  }
}