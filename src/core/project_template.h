#pragma once
#include <memory>
#include <string>
#include <vector>

#include "shared/project_mode.h"

namespace convoy
{

class ProjectTemplate
{
   public:
    std::string name;
    ProjectMode mode;
    ProjectModeConfig config;

    bool load_from_file(const std::string& path);
    bool save_to_file(const std::string& path) const;
};

class TemplateManager
{
   public:
    void load_builtin_templates();
    std::vector<ProjectTemplate> get_templates() const;
    const ProjectTemplate* find(const std::string& name) const;

   private:
    std::vector<ProjectTemplate> templates_;
};

}  // namespace convoy
