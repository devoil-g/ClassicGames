#pragma once

#include <cstdint>
#include <iostream>

#include "Doom/Action/AbstractAction.hpp"
#include "System/Audio/Sound.hpp"

namespace DOOM
{
  namespace EnumAction
  {
    namespace Change
    {
      enum Type
      {
        None,           // No change
        Texture,        // Copy texture from model
        TextureZeroed,  // Copy texture from model and zeroed type
        TextureType,    // Copy texture and type from model
      };

      enum Time
      {
        Before, // Apply change before action
        After   // Apply change after action
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
    std::int16_t  _model; // Model for properties copy

  protected:
    Game::Audio::Sound::Reference _sound; // Sound reference of sector action

    void  changeTexture(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)
    {
      // Change sector texture
      sector.floor_name = doom.level.sectors[_model].floor_name;
      sector.floor_flat = doom.level.sectors[_model].floor_flat;
    }

    void  changeZeroed(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)
    {
      // Zero-ing sector properties
      sector.special = 0;
      sector.damage = 0.f;
    }

    void  changeType(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)
    {
      // Copy properties from model
      sector.special = doom.level.sectors[_model].special;
      sector.damage = doom.level.sectors[_model].damage;
    }

    void  change(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector)
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

        default:  // Should not happen
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
        }
      }
    }

  protected:
    void  remove(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector) override  // Remove action from sector
    {
      // Apply change to sector
      if (ChangeTime == DOOM::EnumAction::Change::Time::After)
        change(doom, sector);

      // Remove action from sector
      sector.action<Type>().reset();
    }

    void  sound(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, DOOM::Doom::Resources::Sound::EnumSound sfx, bool loop = false)  // Emit sound from sector
    {
      int             sector_index = (int)(((uint64_t)&sector - (uint64_t)doom.level.sectors.data()) / sizeof(sector));
      Math::Vector<2> center;
      int             center_nb = 0;

      // Aggregate sector vertexes
      for (const auto& subsector : doom.level.subsectors)
        if (subsector.sector == sector_index)
          for (int segment_index = 0; segment_index < subsector.count; segment_index++) {
            const auto& segment = doom.level.segments[segment_index + subsector.index];

            center += doom.level.vertexes[segment.start];
            center += doom.level.vertexes[segment.end];
            center_nb += 2;
          }

      // No subsector identified
      if (center_nb == 0)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Compute average vertex positions to get the center of sector
      center /= (float)center_nb;

      // Play sound at sector center
      doom.sound(_sound, sfx, Math::Vector<3>(center.x(), center.y(), (sector.ceiling_base + sector.floor_base) / 2), loop);
    }

  public:
    AbstractTypeAction(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, std::int16_t model = -1) :
      DOOM::AbstractAction(doom, sector),
      _model(model),
      _sound(Game::Audio::Sound::Instance().get())
    {
      // Warn for errors
      if (ChangeType != DOOM::EnumAction::Change::Type::None && _model == -1)
        std::cerr << "[DOOM::AbstractTypeAction] Warning, invalid sector model for change." << std::endl;

      // Apply change to sector
      if (ChangeTime == DOOM::EnumAction::Change::Time::Before)
        change(doom, sector);
    }

    virtual ~AbstractTypeAction()
    {
      // Stop looping sounds
      _sound.sound.setLooping(false);
    }
  };
}