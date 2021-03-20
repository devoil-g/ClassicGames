#include "Math/Vector.hpp"

template<>
Math::Vector<3> Math::Vector<3>::cross(const Math::Vector<3>& A, const Math::Vector<3>& B)
{
  return Math::Vector<3>(A.y() * B.z() - A.z() * B.y(), A.z() * B.x() - A.x() * B.z(), A.x() * B.y() - A.y() * B.x());
}
