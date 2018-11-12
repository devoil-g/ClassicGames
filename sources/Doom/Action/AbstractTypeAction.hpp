#ifndef _ABSTRACT_TYPE_ACTION_HPP_
#define _ABSTRACT_TYPE_ACTION_HPP_

#include <iostream>

#include "Doom/Action/AbstractAction.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    namespace Change
    {
      enum Type
      {
	None,		// No change
	Texture,	// Copy texture from model
	TextureZeroed,	// Copy texture from model and zeroed type
	TextureType,	// Copy texture and type from model
      };

      enum Time
      {
	Before,	// Apply change before action
	After	// Apply change after action
      };
    };
  };

  template<
    DOOM::Doom::Level::Sector::Action Type,
    DOOM::EnumAction::Change::Type ChangeType = DOOM::EnumAction::Change::Type::None,
    DOOM::EnumAction::Change::Time ChangeTime = DOOM::EnumAction::Change::Time::Before
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

    void	change(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector)
    {
      // Perform sector properties change
      if (ChangeType != DOOM::EnumAction::Change::Type::None && _model != -1)
      {
	switch (ChangeType) {
	case DOOM::EnumAction::Change::Type::Texture:
	  changeTexture(doom, sector);
	  break;
	case DOOM::EnumAction::Change::Type::TextureZeroed:
	  changeTexture(doom, sector);
	  changeZeroed(doom, sector);
	  break;
	case DOOM::EnumAction::Change::Type::TextureType:
	  changeTexture(doom, sector);
	  changeType(doom, sector);
	  break;

	default:	// Should not happen
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}
      }
    }

  protected:
    void	remove(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector) override	// Remove action from sector
    {
      // Apply change to sector
      if (ChangeTime == DOOM::EnumAction::Change::Time::After)
	change(doom, sector);

      // Remove action from sector
      sector.action<Type>().reset();
    }

  public:
    AbstractTypeAction(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t model = -1) :
      DOOM::AbstractAction(doom, sector),
      _model(model)
    {
      // Warn for errors
      if (ChangeType != DOOM::EnumAction::Change::Type::None && _model == -1)
	std::cerr << "[DOOM::AbstractTypeAction] Warning, invalid sector model for change." << std::endl;

      // Apply change to sector
      if (ChangeTime == DOOM::EnumAction::Change::Time::Before)
	change(doom, sector);
    }

    virtual ~AbstractTypeAction() = default;
  };
};

#endif