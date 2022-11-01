#ifndef _GOG_PROT_DRAG_MANAGER_H_
#define _GOG_PROT_DRAG_MANAGER_H_

#include <json.hpp>
#include <API/UE/Containers/TArray.h>
#include <API/UE/Containers/FString.h>
#include <API/ARK/Ark.h>

namespace Protection
{

	class DragManager
	{
	public:
		DragManager(const nlohmann::json& flyers, const nlohmann::json& grounds, bool carry_flag, bool drag_flag);

		bool CanCarry(APrimalDinoCharacter* dino);
		bool CanDrag(APrimalDinoCharacter* dino);

	private:
		TArray<FString> flyers_exceptions_;
		TArray<FString> grounds_exceptions_;
		bool can_carry_flag_;
		bool can_drag_flag_;
	};

}

#endif // !_GOG_PROT_DRAG_MANAGER_H_