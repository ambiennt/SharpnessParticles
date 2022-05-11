#pragma once

#include <base/log.h>
#include <hook.h>
#include <base/base.h>
#include <yaml.h>
#include <Item/ItemStack.h>
#include <Item/Enchant.h>
#include <Level/Level.h>
#include <Actor/Player.h>
#include <Actor/GameMode.h>
#include <Packet/SpawnParticleEffectPacket.h>
#include <Component/AABBShapeComponent.h>

inline struct Settings {

	std::string particleIdentifier = "uhc:sharpness";

	template <typename IO> static inline bool io(IO f, Settings &settings, YAML::Node &node) {
		return f(settings.particleIdentifier, node["particleIdentifier"]);
	}
} settings;

namespace SharpnessParticles {

inline bool itemHasSharpness(ItemStackBase const &stack) { return EnchantUtils::hasEnchant(Enchant::Type::sharpness, stack); }
bool canInvokeSharpnessParticle(Mob const &attackingMob);
SpawnParticleEffectPacket getSharpnessParticlePacket(Mob const &attackedMob);

} // namespace SharpnessParticles

DEF_LOGGER("SharpnessParticles");