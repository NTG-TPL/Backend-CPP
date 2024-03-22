#pragma once

#include "use_cases.h"
#include "unit_of_work.h"

namespace app {

    class UseCasesImpl : public UseCases {
    public:
        explicit UseCasesImpl(UnitOfWorkFactory& unit_factory) :
                unit_factory_(unit_factory) {}

        void SaveRetiredPlayers(const data_base::domain::RetiredPlayers& records) override;
        data_base::domain::RetiredPlayers GetRetiredPlayers(size_t offset, size_t limit) override;

    private:
        UnitOfWorkFactory& unit_factory_;
    };
} // namespace app