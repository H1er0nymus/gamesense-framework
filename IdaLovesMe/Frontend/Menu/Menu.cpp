#include "Menu.h"
#include "../../Backend/Utilities/Utilities.h"
#include "../../Backend/Config/Settings.h"
#include "../../Backend/Config/Config.h"

#include <algorithm>

using namespace gamesense;

void CMenu::Initialize()
{
	if (this->m_bInitialized)
		return;

	ui::CreateContext();
	GuiContext* g = Globals::Gui_Ctx;
	g->ItemSpacing = Vec2(0, 6);
	g->MenuAlpha = 1;

	CConfig::get()->LoadDefaults();

	Misc::Utilities->Game_Msg("Cheat Initialized!");

	this->m_bIsOpened = true;
	this->m_bInitialized = true;
}

void CMenu::Draw()
{
	CConfig* cfg = CConfig::get();

	static float alpha = 0;
	float fc = Misc::Utilities->GetDeltaTime() * 255 * 10;
	if (!this->m_bIsOpened && alpha > 0.f)
		alpha = std::clamp(alpha - fc / 255.f, 0.f, 1.f);

	if (this->m_bIsOpened && alpha < 1.f)
		alpha = std::clamp(alpha + fc / 255.f, 0.f, 1.f);

	Globals::Gui_Ctx->MenuAlpha = static_cast<int>(floor(alpha * 255));

	if (!this->m_bIsOpened && alpha == 0.f)
		return;

	ui::GetInputFromWindow("Counter-Strike: Global Offensive - Direct3D 9");

	ui::SetNextWindowSize({ 660,560 });
	ui::Begin("Main", GuiFlags_None);

	ui::TabButton("A", &this->m_nCurrentTab, 0, 7); //Ragebot
	ui::TabButton("G", &this->m_nCurrentTab, 1, 7); //AntiAim
	ui::TabButton("B", &this->m_nCurrentTab, 2, 7); //Legitbot
	ui::TabButton("C", &this->m_nCurrentTab, 3, 7); //Visuals
	ui::TabButton("D", &this->m_nCurrentTab, 4, 7); //Misc
	ui::TabButton("E", &this->m_nCurrentTab, 5, 7); //Skins
	ui::TabButton("F", &this->m_nCurrentTab, 6, 7); //Players
	ui::TabButton("H", &this->m_nCurrentTab, 7, 7); //Configs


	switch (this->m_nCurrentTab)
	{

	case 0:
	{
		ui::BeginChild("Other#Rage", { Vec2(6, 0), Vec2(3, 10) });
		{
			ui::Checkbox("Remove recoil", &cfg->b["rage_remove_recoil"]);
			ui::SingleSelect("Accuracy boost", &cfg->i["rage_accuracy_boost"], { "Low", "Medium", "High", "Maximum" });
			ui::Checkbox("Delay shot", &cfg->b["rage_delay_shot"]);
			ui::Checkbox("Quick stop", &cfg->b["rage_quick_stop"]);
			ui::KeyBind("rage_quick_stop_bind", &cfg->i["rage_quick_stop_bind"], &cfg->i["rage_quick_stop_bind_style"]);

			if (cfg->b["rage_quick_stop"])
				ui::MultiSelect("Quick stop options", &cfg->m["rage_quick_stop_options"], { "Early", "Slow motion", "Duck", "Fake duck", "Move between shots", "Ignore molotov", "Taser" });

			ui::Checkbox("Quick peek assist", &cfg->b["rage_quick_peek_assist"]);
			ui::KeyBind("rage_quick_peek_bind", &cfg->i["rage_quick_peek_bind"], &cfg->i["rage_quick_peek_bind_style"]);

			if (cfg->b["rage_quick_peek_assist"]) {
				ui::MultiSelect("Quick peek assist mode", &cfg->m["rage_quick_peek_assist_mode"], { "Retreat on shot", "Retreat on key release" });
				ui::ColorPicker("quick_peek_assist_colorpicker", cfg->c["quick_peek_assist_colorpicker"]);
				ui::SliderInt("Quick peek assist distance", &cfg->i["rage_quick_peek_distance"], 16, 200, cfg->i["rage_quick_peek_distance"] == 200 ? "8" : "%din");
			}

			if (ui::Checkbox("Anti-aim correction", &cfg->b["rage_aa_correction"])) {
				ui::Label("Anti-aim correction override");
				ui::KeyBind("rage_correction_override_bind", &cfg->i["rage_correction_override_bind"], &cfg->i["rage_correction_override_bind_style"]);
			}

			if (cfg->m["rage_target_hitbox"][1] || cfg->m["rage_target_hitbox"][2])
				if (ui::Checkbox("Prefer body aim", &cfg->b["rage_prefer_body_aim"]))
					ui::MultiSelect("Prefer body aim disablers", &cfg->m["rage_other_baim_disablers"], { "Low inaccuray", "Target shot fired", "Target resolved", "Safe point headshot", "Low damage" });

			ui::Label("Force body aim");
			ui::KeyBind("rage_baim_bind", &cfg->i["rage_baim_bind"], &cfg->i["rage_baim_bind_style"]);
			ui::Checkbox("Force body aim on peek", &cfg->b["rage_force_baim"]);
			ui::Label("Duck peek assist");
			ui::KeyBind("rage_fakeduck_bind", &cfg->i["rage_fakeduck_bind"], &cfg->i["rage_fakeduck_bind_style"]);
			ui::Checkbox("Double tap", &cfg->b["rage_doubletap"], true);
			ui::KeyBind("rage_doubletap_bind", &cfg->i["rage_doubletap_bind"], &cfg->i["rage_doubletap_bind_style"]);
			if (cfg->b["rage_doubletap"]) {
				ui::SingleSelect("Double tap mode", &cfg->i["rage_dooubletap_mode"], { "Offensive", "Defensive" });
				ui::SliderInt("Double tap hit chance", &cfg->i["rage_dt_hitchance"], 0, 100, "%d%%");
				ui::SliderInt("Double tap fake lag limit", &cfg->i["rage_dt_fakelag_limit"], 0, 10, "%d");
				ui::MultiSelect("Double tap quick stop", &cfg->m["rage_dt_quick_stop"], { "Slow motion", "Duck", "Move between shots" });
			}
		}
		ui::EndChild();

		ui::BeginChild("Aimbot", { Vec2(0, 0), Vec2(3, 10) });
		{
			ui::Checkbox("Enabled", &cfg->b["rage_enabled"]);
			ui::KeyBind("rage_enabled_bind_label", &cfg->i["rage_enabled_bind"], &cfg->i["rage_enabled_bind_style"]);
			ui::SingleSelect("Target selection", &cfg->i["rage_target_selection"], { "Highest damage", "Cycle", "Cycle (2x)", "Near crosshair", "Best hit chance" });
			ui::MultiSelect("Target hitbox", &cfg->m["rage_target_hitbox"], { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" });
			ui::MultiSelect("Multi-point", &cfg->m["rage_multi_point"], { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" });

			if (std::find_if(std::begin(cfg->m["rage_multi_point"]), std::end(cfg->m["rage_multi_point"]), [](auto p) { return p.second == true; }) != std::end(cfg->m["rage_multi_point"])) {
				ui::SingleSelect("", &cfg->i["rage_multi_point_amount"], { "Low", "Medium", "High" });
				ui::SliderInt("Multi-point scale", &cfg->i["rage_multi_point_scale"], 24, 100, cfg->i["rage_multi_point_scale"] == 24 ? "Auto" : "%d%%");
			}
			ui::KeyBind("rage_multi_point_label", &cfg->i["rage_multi_point_bind"], &cfg->i["rage_multi_point_bind_style"]);
			ui::Checkbox("Prefer safe point", &cfg->b["rage_prefer_safe_point"]);
			ui::Label("Force safe point");
			ui::KeyBind("rage_prefer_safe_point_label", &cfg->i["rage_prefer_safe_point_bind"], &cfg->i["rage_prefer_safe_point_bind_style"]);
			ui::MultiSelect("Avoid unsafe hitboxes", &cfg->m["rage_avoid_unsafe_hitboxes"], { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" });
			ui::Checkbox("Automatic fire", &cfg->b["rage_autofire"]);
			ui::Checkbox("Automatic penetration", &cfg->b["rage_auto_penetration"]);
			ui::Checkbox("Silent aim", &cfg->b["rage_silent_aim"]);
			ui::SliderInt("Minimum hit chance", &cfg->i["rage_hitchance"], 0, 100, cfg->i["rage_hitchance"] < 1 ? "Off" : "%d%%");
			ui::SliderInt("Minimum damage", &cfg->i["rage_mindmg"], 0, 126, cfg->i["rage_mindmg"] == 0 ? "Auto" : (cfg->i["rage_mindmg"] > 100 ? "HP+%d" : "%d"), (cfg->i["rage_mindmg"] > 100 ? 100 : 0));
			ui::Checkbox("Automatic scope", &cfg->b["rage_autoscope"]);
			ui::Checkbox("Reduce aim step", &cfg->b["rage_reduce_aimstep"]);
			ui::SliderInt("Maximum FOV", &cfg->i["rage_fov"], 0, 180, "%d°");
			ui::Checkbox("Log misses due to spread", &cfg->b["rage_log_misses"]);
			ui::MultiSelect("Low FPS mitigations", &cfg->m["rage_fps_mitigations"], { "Force low accuracy boost", "Disable multipoint: feet", "Disable multipoint: arms", "Disable multipoint: legs", "Disable hitbox: feet", "Force low multipoint", "Lower hit chance precision", "Limit targets per tick" });
		}
		ui::EndChild();

		break;
	}

	case 1:
	{
		ui::BeginChild("Anti-aimbot angles");
		{
			ui::Checkbox("Enabled", &cfg->b["aa_enabled"]);
			ui::SingleSelect("Pitch", &cfg->i["aa_pitch"], { "Off", "Default", "Up", "Down", "Minimal", "Random" });
			ui::SingleSelect("Yaw base", &cfg->i["aa_yaw_base"], { "Local view", "At targets" });

			ui::SingleSelect("Yaw", &cfg->i["aa_yaw"], { "Off", "180", "Spin", "Static", "180 Z", "Crosshair" });

			switch (cfg->i["aa_yaw"]) {
			case 1:
				ui::SliderInt("", &cfg->i["aa_yaw_180"], -180, 180, "%d°"); break;
			case 2:
				ui::SliderInt("", &cfg->i["aa_yaw_spin"], -180, 180, "%d°"); break;
			case 3:
				ui::SliderInt("", &cfg->i["aa_yaw_static"], -180, 180, "%d°"); break;
			case 4:
				ui::SliderInt("", &cfg->i["aa_yaw_180Z"], -180, 180, "%d°"); break;
			case 5:
				ui::SliderInt("", &cfg->i["aa_yaw_Crosshair"], -180, 180, "%d°"); break;
			}

			if (cfg->i["aa_yaw"] != 0) {
				ui::SingleSelect("Yaw jitter", &cfg->i["aa_yaw_jitter"], { "Off", "Offset", "Center", "Random" });
				if (cfg->i["aa_yaw_jitter"] != 0)
					ui::SliderInt("a", &cfg->i["aa_yaw_jitter_amount"], -180, 180, "%d°");
			}
			ui::SingleSelect("Body yaw", &cfg->i["aa_body_yaw"], { "Off", "Opposite", "Jitter", "Static" });

			switch (cfg->i["aa_body_yaw"]) {
			case 2:	ui::SliderInt("", &cfg->i["aa_body_yaw_jitter_amount"], -180, 180, "%d°"); break;
			case 3: ui::SliderInt("", &cfg->i["aa_body_yaw_static_amount"], -180, 180, "%d°"); break;
			}

			if (cfg->i["aa_body_yaw"] != 0) {
				ui::Checkbox("Freestanding body yaw", &cfg->b["aa_freestanding_body_yaw"]);
				ui::SliderInt("", &cfg->i["aa_fake_yaw_limit"], 0, 60, "%d°");
			}

			ui::Checkbox("Edge yaw", &cfg->b["aa_edge_yaw"]);
			ui::KeyBind("aa_edge_yaw_bind", &cfg->i["aa_edge_yaw_bind"], &cfg->i["aa_edge_yaw_bind_style"]);
			ui::MultiSelect("Freestanding", &cfg->m["aa_freestanding"], { "Default" });
		}
		ui::EndChild();

		ui::BeginChild("Fake lag", { Vec2(6,0), Vec2(3, 5) }); {
			ui::Checkbox("Enabled", &cfg->b["aa_fakelag_enabled"]);
			ui::KeyBind("aa_fakelag_bind", &cfg->i["aa_fakelag_bind"], &cfg->i["aa_fakelag_bind_style"]);
			ui::SingleSelect("Amount", &cfg->i["aa_fakelag_amount"], { "Dynamic", "Maximum", "Fluctuate" });
			ui::SliderInt("Variance", &cfg->i["aa_fakelag_variance"], 0, 100, "%d%%");
			ui::SliderInt("Limit", &cfg->i["aa_fakelag_limit"], 0, 15, "%d");
		}
		ui::EndChild();

		ui::BeginChild("Other#AA", { Vec2(6,7), Vec2(3, 3) });
		{
			ui::Checkbox("Slow motion", &cfg->b["aa_other_slow_motion"]);
			ui::KeyBind("aa_other_slow_motion_bind", &cfg->i["aa_other_slow_motion_bind"], &cfg->i["aa_other_slow_motion_bind_style"]);
			ui::SingleSelect("Slow motion type", &cfg->i["aa_other_slow_motion_type"], { "Favor high speed", "Favor anti-aim" });
			ui::SingleSelect("Leg movement", &cfg->i["aa_other_leg_movement"], { "Off", "Always slide", "Never slide" });
			ui::Checkbox("On shot anti-aim", &cfg->b["aa_onshotaa"], true);
			ui::KeyBind("aa_other_onshot_bind", &cfg->i["aa_other_onshot_bind"], &cfg->i["aa_other_onshot_bind_style"]);
			ui::Label("Fake peek", true);
			ui::KeyBind("aa_other_fakepeek_bind", &cfg->i["aa_other_fakepeek_bind"], &cfg->i["aa_other_fakepeek_bind_style"]);
		}
		ui::EndChild();

		break;
	}

	case 2:
	{
		ui::BeginChild("Weapon type#Legit", { Vec2(0,0), Vec2(9, 0) }, GuiFlags_NoMove | GuiFlags_NoResize);
		ui::TabButton("G", &this->m_nCurrentLegitTab, 0, 6, GuiFlags_LegitTab);
		ui::TabButton("P", &this->m_nCurrentLegitTab, 1, 6, GuiFlags_LegitTab);
		ui::TabButton("W", &this->m_nCurrentLegitTab, 2, 6, GuiFlags_LegitTab);
		ui::TabButton("d", &this->m_nCurrentLegitTab, 3, 6, GuiFlags_LegitTab);
		ui::TabButton("f", &this->m_nCurrentLegitTab, 4, 6, GuiFlags_LegitTab);
		ui::TabButton("a", &this->m_nCurrentLegitTab, 5, 6, GuiFlags_LegitTab);
		ui::EndChild();

		ui::BeginChild("Aimbot#Legit", { Vec2(0, 2), Vec2(3, 8) });
		ui::Checkbox("Enabled", &cfg->b["legit_enabled"]);
		ui::KeyBind("legit_enabled_label", &cfg->i["legit_enabled_bind"], &cfg->i["legit_enabled_bind_style"]);
		ui::SliderInt("Speed", &cfg->i["legit_speed"], 0, 7, cfg->i["legit_speed"] < 1 ? "0" : "%d");
		ui::SliderInt("Speed (in attack)", &cfg->i["legit_speed_attack"], 0, 7, cfg->i["legit_speed_attack"] < 1 ? "0" : "%d");
		ui::SliderInt("Speed scale - FOV", &cfg->i["legit_speed_scale"], 0, 100, cfg->i["legit_speed_scale"] < 1 ? "Off" : "%d%%");
		ui::SliderInt("Maximum lock-on time", &cfg->i["legit_max_lock_time"], 0, 100, cfg->i["legit_max_lock_time"] < 1 ? "0ms" : "%dms");
		ui::SliderInt("Reaction time", &cfg->i["legit_reaction_time"], 0, 200, cfg->i["legit_reaction_time"] < 1 ? "0ms" : "%dms");
		ui::SliderInt("Maximum FOV", &cfg->i["legit_fov"], 0, 100, "%d°");
		ui::SliderInt("Recoil compensation (P)", &cfg->i["legit_recoil_compensation"], 0, 100, "%d%%");
		ui::SliderInt("Recoil compensation (Y)", &cfg->i["legit_recoil_compensation_y"], 0, 100, "%d%%");
		ui::Checkbox("Quick stop", &cfg->b["legit_quick_stop"]);
		ui::Checkbox("Aim though smoke", &cfg->b["legit_aim_though_smoke"]);
		ui::Checkbox("Aim while blind", &cfg->b["legit_aim_while_blind"]);
		ui::Checkbox("Head", &cfg->b["legit_head"]);
		ui::Checkbox("Chest", &cfg->b["legit_chest"]);
		ui::Checkbox("Stomach", &cfg->b["legit_stomach"]);
		ui::EndChild();

		ui::BeginChild("Triggerbot#Legit", { Vec2(6, 2), Vec2(3, 5) });
		ui::Checkbox("Enabled", &cfg->b["legit_trigger_enabled"]);
		ui::KeyBind("legit_trigger_enabled_label", &cfg->i["legit_trigger_enabled_bind"], &cfg->i["legit_trigger_enabled_bind_style"]);
		ui::SliderInt("Minimum hit chance", &cfg->i["legit_trigger_hitchance"], 0, 100, cfg->i["legit_trigger_hitchance"] < 1 ? "Off" : "%d%%");
		ui::SliderInt("Reaction time", &cfg->i["legit_trigger_reaction_time"], 0, 200, cfg->i["legit_trigger_reaction_time"] < 1 ? "0ms" : "%dms");
		ui::Checkbox("Burst fire", &cfg->b["legit_trigger_burst_fire"]);
		ui::SliderInt("Minimum damage", &cfg->i["legit_trigger_mindmg"], 0, 126, cfg->i["legit_trigger_mindmg"] == 0 ? "1" : (cfg->i["legit_trigger_mindmg"] > 100 ? "HP+%d" : "%d"), (cfg->i["legit_trigger_mindmg"] > 100 ? 100 : 0));
		ui::Checkbox("Automatic penetration", &cfg->b["legit_trigger_automatic_penetration"]);
		ui::Checkbox("Shoot though smoke", &cfg->b["legit_trigger_shoot_though_smoke"]);
		ui::Checkbox("Shoot while blind", &cfg->b["legit_trigger_shoot_while_blind"]);
		ui::Checkbox("Head", &cfg->b["legit_trigger_head"]);
		ui::Checkbox("Chest", &cfg->b["legit_trigger_chest"]);
		ui::Checkbox("Stomach", &cfg->b["legit_trigger_stomach"]);
		ui::EndChild();

		ui::BeginChild("Other#Legit", { Vec2(6, 9), Vec2(3, 1) });
		ui::SingleSelect("Accuracy boost", &cfg->i["legit_accuracy_boost"], { "Low", "Medium", "High", "Maximum" });
		ui::Checkbox("Standalone recoil compensation", &cfg->b["legit_standalone_recoil_compensation"]);
		ui::EndChild();


		break;
	}

	case 3:
	{
		ui::BeginChild("Player ESP", { Vec2(0,0), Vec2(3, 6) });
		ui::EndChild();
		ui::BeginChild("Colored models", { Vec2(0, 8), Vec2(3, 2) });
		ui::EndChild();
		ui::BeginChild("Other ESP", { Vec2(6, 0), Vec2(3, 3) });
		ui::EndChild();
		ui::BeginChild("Effects", { Vec2(6, 5), Vec2(3, 5) });
		ui::EndChild();

		break;
	}

	case 4:
	{
		ui::BeginChild("Miscellaneous", { Vec2(0,0), Vec2(3, 10) });
		ui::EndChild();
		ui::BeginChild("Movement", { Vec2(6, 0), Vec2(3, 4) });
		ui::EndChild();
		ui::BeginChild("Settings", { Vec2(6, 6), Vec2(3, 4) });
		ui::Label("Menu key");
		ui::KeyBind("misc_menu_key", &cfg->i["menu_key"], &cfg->i["misc_menu_keystyle"]);
		ui::Label("Menu color");
		ui::ColorPicker("MenuColor", cfg->c["MenuColor"]);
		ui::SingleSelect("DPI scale", &cfg->i["menu_scale"], { "100%", "125%", "150%", "175%", "200%" });
		ui::Checkbox("Anti-untrusted", &cfg->b["misc_anti_untrusted"]);
		ui::Checkbox("Hide from OBS", &cfg->b["misc_hide_from_obs"]);
		ui::Checkbox("Low FPS warning", &cfg->b["misc_low_fps_warning"]);
		ui::Checkbox("Lock menu layout", &cfg->b["misc_lock_menu_layout"]);
		if (ui::Button("Reset menu layout"))
			Settings->ResetLayout = true;
		else
			Settings->ResetLayout = false;

		if (ui::Button("Unload"))
			Settings->UnloadCheat = true;
		ui::EndChild();

		break;
	}

	case 5:
	{
		ui::BeginChild("Model options", { Vec2(0,0), Vec2(3, 10) });
		ui::EndChild();
		ui::BeginChild("Weapon skin", { Vec2(6, 0), Vec2(3, 10) });
		ui::EndChild();

		break;
	}

	case 6:
	{
		ui::BeginChild("Players", { Vec2(0,0), Vec2(3, 10) });
		ui::EndChild();
		ui::BeginChild("Adjustments", { Vec2(6, 0), Vec2(3, 10) });
		ui::EndChild();

		break;
	}

	case 7:
	{
		ui::BeginChild("Presets", { Vec2(0,0), Vec2(3, 10) });

		if (ui::BeginListbox("ConfigsList"))
		{
			for (auto config : CConfig::get()->List) {
				if (ui::Selectable(config.c_str(), CConfig::get()->Current == config.c_str()))
					strcpy_s(cfg->s["config_name"], config.c_str());
			}
		}
		ui::EndListbox();

		ui::InputText("cfg_input", cfg->s["config_name"]);

		if (ui::Button("Load"))
			CConfig::get()->Load();

		if (ui::Button("Save"))
			CConfig::get()->Save();

		if (ui::Button("Delete"))
			CConfig::get()->Delete();

		if (ui::Button("Reset"))
			CConfig::get()->LoadDefaults();

		ui::Button("Import from clipboard");
		ui::Button("Export to clipboard");

		ui::EndChild();
		ui::BeginChild("Lua", { Vec2(6, 0), Vec2(3, 10) });
		ui::EndChild();

		break;
	}

	case 8:
	{
		ui::BeginChild("A", { Vec2(0,0), Vec2(3, 10) });
		ui::EndChild();
		ui::BeginChild("B", { Vec2(6, 0), Vec2(3, 10) });
		ui::EndChild();
	}
	}

	ui::End();
}

bool CMenu::IsMenuOpened()
{
	return this->m_bIsOpened;
}

void CMenu::SetMenuOpened(bool v)
{
	this->m_bIsOpened = v;
}

D3DCOLOR CMenu::GetMenuColor() {
	GuiContext* g = Globals::Gui_Ctx;
	CConfig* cfg = CConfig::get();
	return D3DCOLOR_RGBA(cfg->c["MenuColor"][0], cfg->c["MenuColor"][1], cfg->c["MenuColor"][2], min(cfg->c["MenuColor"][3], g->MenuAlpha));
}