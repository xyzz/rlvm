#include "machine/rlvm_instance.h"

class AndroidRLVMInstance : public RLVMInstance {
 public:
  void ReloadAllTextures();
 protected:
  virtual bool AskUserPrompt(const std::string& message_text,
                             const std::string& informative_text,
                             const std::string& true_button,
                             const std::string& false_button);
};
