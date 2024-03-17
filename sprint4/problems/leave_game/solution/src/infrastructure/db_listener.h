#pragma once

#include "../app/application.h"
#include "../app_serialize/ser_app.h"
#include "../logger/logger.h"
#include "../db/postgres.h"
#include "../app/use_cases_impl.h"

namespace infrastructure {
    class DataBaseListener: public app::ApplicationListener {
    public:
        DataBaseListener(app::Application& application, data_base::postgres::Database& db):
            application_(application), db_(db) {
        }

        void OnTick(std::chrono::milliseconds tick) override {
            using namespace std::string_literals;
            try {
                RetrivePlayers();
            } catch (const std::system_error& e) {
                server_logging::Logger::LogError(e.code(), "Delete players error: "s + e.what());
                throw std::runtime_error("Delete players error:"s + e.what());
            } catch (const std::exception& e) {
                throw std::runtime_error("Delete players error:"s + e.what());
            }
        }

        void RetrivePlayers() {
            auto& game = application_.GetGameModel();
            auto& players = application_.GetPlayers();
            auto retirement_time =  game.GetDogRetirementTime();
            std::vector<std::pair<app::Token, std::shared_ptr<app::Player>>> to_delete;
            std::vector<data_base::domain::RetiredPlayer> to_save;
            for (auto& [token, player]: players.GetPlayerTokens().GetTokenToPlayer()) {
                if(player != nullptr){
                    auto dog = player->GetDog();
                    if (dog != nullptr && dog->GetStayTime() >= retirement_time) {
                        to_save.emplace_back(player->GetId(),
                                             dog->GetName(),
                                             dog->GetScore(),
                                             dog->GetLifeTime());
                        to_delete.emplace_back(token, player);
                    }
                }
            }
            use_cases_.SaveRetiredPlayers(to_save);
            for (auto& [token, player] : to_delete) {
                if(player != nullptr){
                    auto& sesseion = player->GetSession();
                    sesseion.DeleteDog(player->GetDog()->GetId());
                    players.DeleteByToken(token);
                }
            }
        }

    private:
        app::Application& application_;
        data_base::postgres::Database& db_;
        app::UseCasesImpl use_cases_{db_.GetUnitOfWorkFactory()};
    };
}