#pragma once

/*Maxsu's block spark function*/
namespace MaxsuBlockSpark {
    class blockSpark
    {
    public:
        static void playPerfectBlockSpark(RE::Actor* defender) {
            if(!defender || !defender->GetActorRuntimeData().currentProcess || !defender->GetActorRuntimeData().currentProcess->high || !defender->Get3D()) {
                return;
            }
            auto GetBipeObjIndex = [](RE::TESForm* parryEquipment, bool rightHand) -> RE::BIPED_OBJECT {
                if(!parryEquipment)
                    return RE::BIPED_OBJECT::kNone;

                if(auto weap = parryEquipment->As<RE::TESObjectWEAP>()) {
                    switch(weap->GetWeaponType()) {
                        case RE::WEAPON_TYPE::kOneHandSword:
                            return rightHand ? RE::BIPED_OBJECT::kOneHandSword : RE::BIPED_OBJECT::kShield;
                        case RE::WEAPON_TYPE::kOneHandAxe:
                            return rightHand ? RE::BIPED_OBJECT::kOneHandAxe : RE::BIPED_OBJECT::kShield;
                        case RE::WEAPON_TYPE::kOneHandMace:
                            return rightHand ? RE::BIPED_OBJECT::kOneHandMace : RE::BIPED_OBJECT::kShield;
                        case RE::WEAPON_TYPE::kOneHandDagger:
                            return rightHand ? RE::BIPED_OBJECT::kOneHandDagger : RE::BIPED_OBJECT::kShield;
                        case RE::WEAPON_TYPE::kTwoHandAxe:
                        case RE::WEAPON_TYPE::kTwoHandSword:
                        case RE::WEAPON_TYPE::kHandToHandMelee:
                            return RE::BIPED_OBJECT::kTwoHandMelee;
                        case RE::WEAPON_TYPE::kBow:
                            return RE::BIPED_OBJECT::kBow;
                        case RE::WEAPON_TYPE::kStaff:
                            return RE::BIPED_OBJECT::kStaff;
                        case RE::WEAPON_TYPE::kCrossbow:
                            return RE::BIPED_OBJECT::kCrossbow;
                        default:
                            break;
                    }
                } else if(parryEquipment->IsArmor())
                    return RE::BIPED_OBJECT::kShield;

                return RE::BIPED_OBJECT::kNone;
            };
            RE::BIPED_OBJECT BipeObjIndex;
            auto             defenderLeftEquipped = defender->GetEquippedObject(true);

            if(defenderLeftEquipped && (defenderLeftEquipped->IsWeapon() || defenderLeftEquipped->IsArmor())) {
                BipeObjIndex = GetBipeObjIndex(defenderLeftEquipped, false);
            } else {
                BipeObjIndex = GetBipeObjIndex(defender->GetEquippedObject(false), true);
            }

            if(BipeObjIndex == RE::BIPED_OBJECT::kNone) {
                return;
            }

            auto defenderNode = defender->GetCurrentBiped()->objects[BipeObjIndex].partClone;
            if(!defenderNode || !defenderNode.get()) {
                return;
            }

            const auto modelName =
                BipeObjIndex == RE::BIPED_OBJECT::kShield && defenderLeftEquipped && defenderLeftEquipped->IsArmor() ? "ValhallaCombat\\impactShieldRoot.nif" : "ValhallaCombat\\impactWeaponRoot.nif";

            RE::NiPoint3 sparkPos = defenderNode->worldBound.center;

            //DEBUG("Get Weapon Spark Position!");
            RE::BSTempEffectParticle::Spawn(defender->GetParentCell(), 0.0f, modelName, defenderNode->world.rotate, sparkPos, 1.0f, 4U, defenderNode.get());
        }
    };
}  // namespace MaxsuBlockSpark

