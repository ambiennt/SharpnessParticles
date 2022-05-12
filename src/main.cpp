#include "main.h"
#include <dllentry.h>

void dllenter() {}
void dllexit() {}

DEFAULT_SETTINGS(settings);

namespace SharpnessParticles {

bool canInvokeSharpnessParticle(Mob const &attackingMob) {

	if (!attackingMob.isInstanceOfPlayer()) return true;

	auto& player = *(Player*)&attackingMob;
	uint64_t currentTick = player.mLevel->getServerTick();
	return ((currentTick - player.EZPlayerFields->mLastSharpnessParticleEmitTimestamp) >= 2); // cooldown of 2 ticks for players
}

SpawnParticleEffectPacket getSharpnessParticlePacket(Mob const &attackedMob) {
	const auto &aabb = attackedMob.mAABBComponent.mAABB;
	const auto &pos  = attackedMob.getPos();
	float newPosY = attackedMob.getPosGrounded().y + ((float)(aabb.max.y - aabb.min.y) * 0.6f);

	return SpawnParticleEffectPacket((uint8_t)(attackedMob.mDimensionId),
		ActorUniqueID(-1), Vec3(pos.x, newPosY, pos.z), settings.particleIdentifier);
}

} // namespace SharpnessParticles

TInstanceHook(void, "?attack@GameMode@@UEAA_NAEAVActor@@@Z", GameMode, Actor &target) {
	original(this, target);

	if (target.isInstanceOfMob()) {

		auto& attacker = *this->mPlayer;
		bool usingSharpnessItem = SharpnessParticles::itemHasSharpness(attacker.getCarriedItem());
		if (usingSharpnessItem) {

			if (SharpnessParticles::canInvokeSharpnessParticle(attacker)) {

				auto pkt = SharpnessParticles::getSharpnessParticlePacket(*(Mob*)&target);
				attacker.sendNetworkPacket(pkt);

				attacker.EZPlayerFields->mLastSharpnessParticleEmitTimestamp = attacker.mLevel->getServerTick();
			}
		}
	}
}

TInstanceHook(void, "?actuallyHurt@Player@@UEAAXHAEBVActorDamageSource@@_N@Z",
	Player, int32_t dmg, ActorDamageSource &source, bool bypassArmor) {
	original(this, dmg, source, bypassArmor);

	if (source.isEntitySource() && !source.isChildEntitySource()) {

		auto& lvl = *this->mLevel;
		auto attacker = lvl.fetchEntity(source.getEntityUniqueID(), false);

		if (attacker && attacker->isInstanceOfMob()) {

			bool usingSharpnessItem = SharpnessParticles::itemHasSharpness(attacker->getCarriedItem());
			if (usingSharpnessItem) {

				uint64_t currentTick = lvl.getServerTick();
				if ((currentTick - this->mLastHurtTimestamp) >= 2) { // cooldown of 2 ticks for players

					auto pkt = SharpnessParticles::getSharpnessParticlePacket(*this);
					this->sendNetworkPacket(pkt);
				}
			}
		}
	}
}