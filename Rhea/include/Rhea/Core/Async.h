#ifndef RHEA_CORE_ASYNC_H
#define RHEA_CORE_ASYNC_H

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace rhea
{
    template<class Context>
    class ContextRunner
        : public std::enable_shared_from_this<ContextRunner<Context>>
    {
    public:
        using Completion = std::function<void(rhea::ErrorReport)>;
        using Handler = std::function<void(Context*, Completion)>;

        // Liefert bei ungültigem/abgebrochenem Auftrag einen Fehler.
        using Validate = std::function<rhea::ErrorReport()>;

        static void run(
            std::shared_ptr<Context> context,
            std::vector<Handler> handlers,
            Validate validate,
            Completion completion)
        {
            auto runner = std::shared_ptr<ContextRunner>(
                new ContextRunner(
                    std::move(context),
                    std::move(handlers),
                    std::move(validate),
                    std::move(completion)));

            runner->advance();
        }

    private:
        ContextRunner(
            std::shared_ptr<Context> context,
            std::vector<Handler> handlers,
            Validate validate,
            Completion completion)
            : m_context(std::move(context)),
            m_handlers(std::move(handlers)),
            m_validate(std::move(validate)),
            m_completion(std::move(completion))
        {}

        void advance()
        {
            if (m_finished)
                return;

            if (m_validate)
                m_report.merge(m_validate());

            if (m_report.isError() || m_index == m_handlers.size()) {
                finish();
                return;
            }

            // Lokale Kopie bleibt auch bei synchronem Completion gültig.
            auto handler = m_handlers[m_index++];
            auto context = m_context;
            auto self = this->shared_from_this();

            // Jeder einzelne Handler darf seine Fortsetzung nur einmal auslösen.
            auto completed = std::make_shared<bool>(false);

            handler(context.get(),
                [self, context, completed](rhea::ErrorReport report) mutable {
                    if (*completed || self->m_finished)
                        return;

                    *completed = true;
                    self->m_report.merge(std::move(report));
                    self->advance();
                });
        }

        void finish()
        {
            if (m_finished)
                return;

            m_finished = true;

            auto completion = std::move(m_completion);
            auto report = std::move(m_report);

            if (completion)
                completion(std::move(report));
        }

        std::shared_ptr<Context> m_context;
        std::vector<Handler> m_handlers;
        Validate m_validate;
        Completion m_completion;
        rhea::ErrorReport m_report;

        size_t m_index = 0;
        bool m_finished = false;
    };
}

#endif // !RHEA_CORE_ASYNC_H