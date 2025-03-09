#include "emojitextdocument.h"

std::vector<std::pair<unsigned int, unsigned int>> EmojiTextDocument::getEmojiPos(std::string const &str) const
{
  // for (char c : str)
  // {
  //   if (std::isprint(c))
  //     std::cout << c;
  //   else
  //     std::cout << std::hex << static_cast<int>(c & 0xff);
  // }
  // std::cout << "" << std::endl;

  std::vector<std::pair<unsigned int, unsigned int>> results;

  for (unsigned int i = 0; i < std::max(static_cast<unsigned int>(str.size()), s_emoji_min_size) - s_emoji_min_size; ++i)
  {
    //std::cout << "Checking byte " << std::dec << i << ": " << std::hex << static_cast<int>(str[i] & 0xff) << std::endl;
    if (s_emoji_first_bytes.find(str[i]) != s_emoji_first_bytes.end())
    {
      for (char const *const emoji_string : s_emoji_unicode_list)
      {
        int emoji_size = std::strlen(emoji_string);
        if (i <= (str.size() - emoji_size) &&
            std::strncmp(str.data() + i, emoji_string, emoji_size) == 0)
        {

          // std::cout << "matched emoji: ";
          // for (unsigned int c = 0; c < emoji_size; ++c)
          //   std::cout << std::hex << static_cast<int>(emoji_string[c] & 0xff);
          // std::cout << "" << std::endl;

          results.emplace_back(std::make_pair(i, emoji_size));
          i += emoji_size - 1; // minus one because ++i in for loop

          //str->insert(i, "<*>");
          //str->insert(i + 3 + emoji_size, "<*>");
          //i += 3 + emoji_size + 3;

          //std::cout << *str << std::endl;
        }
      }
    }
    //else if ((*str)[i] != ' ') // spaces don't count
    //  all_emoji = false;
  }
  return results;
}
