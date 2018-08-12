#ifndef _ABSTRACT_TYPE_ACTION_HPP_
#define _ABSTRACT_TYPE_ACTION_HPP_

#include "Doom/Action/AbstractAction.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    enum Change
    {
      ChangeNone,		// No change
      ChangeTexture,		// Copy texture from model
      ChangeTextureZeroed,	// Copy texture from model and zeroed type
      ChangeTextureType,	// Copy texture and type from model
    };
  };

  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumAction::Change Change = DOOM::EnumAction::Change::ChangeNone
  >
  class AbstractTypeAction : public DOOM::AbstractAction
  {
  private:
    int16_t	_model;	// Model for properties copy

    void	changeTexture(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)
    {
      // Change sector texture
      sector.floor_name = doom.level.sectors[_model].floor_name;
      sector.floor_flat = doom.level.sectors[_model].floor_flat;
    }

    void	changeZeroed(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)
    {
      // Zero-ing sector properties
      sector.special = 0;
      sector.damage = 0.f;
    }

    void	changeType(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)
    {
      // Copy properties from model
      sector.special = doom.level.sectors[_model].special;
      sector.damage = doom.level.sectors[_model].damage;
    }

  protected:
    void	remove(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector) override	// Remove action from sector
    {
      // Perform sector properties change
      if (Change != DOOM::EnumAction::Change::ChangeNone && _model != -1)
      {
	switch (Change) {
	case DOOM::EnumAction::Change::ChangeTexture:
	  changeTexture(doom, sector);
	  break;
	case DOOM::EnumAction::Change::ChangeTextureZeroed:
	  changeTexture(doom, sector);
	  changeZeroed(doom, sector);
	  break;
	case DOOM::EnumAction::Change::ChangeTextureType:
	  changeTexture(doom, sector);
	  changeType(doom, sector);
	  break;

	default:	// Should not happen
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}
      }

      // Remove action from sector
      sector.action<Type>().reset();
    }

  public:
    AbstractTypeAction(DOOM::Doom & doom, int16_t model = -1) :
      AbstractAction(doom),
      _model(model)
    {
      // Warn for errors
      if (Change != DOOM::EnumAction::Change::ChangeNone && _model == -1)
	std::cerr << "[DOOM::AbstractTypeAction] Warning, invalid sector model for change." << std::endl;
    }

    virtual ~AbstractTypeAction() = default;
  };
};

#endif